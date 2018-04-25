/*
 * thread_safe_event.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Konrad Frederic <fred.konrad@greensocs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

#ifndef THREAD_SAFE_EVENT_H
#define THREAD_SAFE_EVENT_H

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <cerrno>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <systemc.h>
#include <time.h>
#include <tlm_utils/tlm_quantumkeeper.h>

//#define SC_HAS_ASYNC_ATTACH_SUSPENDING
#define DECOUPLED

#define COUT if (0) cout << pthread_self() << ":"

namespace gs {
namespace gt {

/** Convenience semaphore implementation */
class sem_i {
    sem_t sem;

  public:
    sem_i(int i) { sem_init(&sem, 0, i); }
    sem_t &operator()() { return sem; }
    void wait() {
        while (sem_wait(&sem) != 0) {
        };
    }
    void post() { sem_post(&sem); }
};

/** Convenience mutex implementation */
class spin_mutex {
    pthread_spinlock_t mtx;

  public:
    spin_mutex() { pthread_spin_init(&mtx, 0); }
    void lock() { pthread_spin_lock(&mtx); }
    void unlock() { pthread_spin_unlock(&mtx); }
};

/** Convenience timed cond implementation */
class timed_cond {
    pthread_cond_t cnd;
    pthread_mutex_t mutex;
    int state;

  public:
    timed_cond() {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cnd, NULL);
    }
    bool wait() {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1; // Wait 1 second
        pthread_mutex_lock(&mutex);
        int p = pthread_cond_timedwait(&cnd, &mutex, &ts);
        pthread_mutex_unlock(&mutex);
        return p != 0;
    }

    void release() {
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&cnd);
        pthread_mutex_unlock(&mutex);
    }
};

/** Implementation of 'end of delta' */
class before_end_of_delta_helper_if {
  public:
    virtual void simulation_phase_callback() = 0;
};

class before_end_of_delta_helper : sc_core::sc_module {
    before_end_of_delta_helper_if *parent;
    sc_core::sc_event endofdeltaEvent;
    void endofdeltaMethod() {
        if (sc_pending_activity()) {
            endofdeltaEvent.notify(sc_time_to_pending_activity() + sc_time(0, SC_PS));
        } else {
            if (parent) {
                parent->simulation_phase_callback();
            }
            // ensure events get back into the kernel
            endofdeltaEvent.notify(sc_time(0, SC_PS));
        }
    }

  public:
    SC_HAS_PROCESS(before_end_of_delta_helper);
    before_end_of_delta_helper(sc_module_name n, before_end_of_delta_helper_if *p) {
        parent = p;
        SC_METHOD(endofdeltaMethod);
        sensitive << endofdeltaEvent;
    }
};

// ----------------------------------------------------------------------------
//  CLASS : event_async
//
//  Class for notifying event asyncronously
// ----------------------------------------------------------------------------
class gs_event_async : public sc_core::sc_prim_channel, public sc_event, before_end_of_delta_helper_if {

    static sem_i global_semaphore;
#ifndef SC_HAS_ASYNC_ATTACH_SUSPENDING
    static before_end_of_delta_helper *helper;
#endif

  private:
    sc_core::sc_time m_delay;
    spin_mutex local_mutex;
    int outstanding;

  public:
    gs_event_async(const char *name = "") : local_mutex(), outstanding(0) {
#ifdef SC_HAS_ASYNC_ATTACH_SUSPENDING
        async_attach_suspending();
#else
        if (!helper) {
            helper = new before_end_of_delta_helper("before_end_of_delta_helper", this);
        }
#endif
    }

    void notify(sc_core::sc_time delay = SC_ZERO_TIME) {
#ifndef SC_HAS_ASYNC_ATTACH_SUSPENDING
        local_mutex.lock();
#endif
        m_delay = delay;
        async_request_update();
#ifndef SC_HAS_ASYNC_ATTACH_SUSPENDING
        global_semaphore.post(); // post token to ensure SystemC execution
        outstanding++;
        local_mutex.unlock();
#endif
    }

  protected:
    void update(void) {
// we should be in SystemC thread
#ifndef SC_HAS_ASYNC_ATTACH_SUSPENDING
        local_mutex.lock();
#endif
        sc_event::notify(m_delay);
#ifndef SC_HAS_ASYNC_ATTACH_SUSPENDING
        for (; outstanding > 0; outstanding--) {
            global_semaphore.wait();
        }
        local_mutex.unlock();
#endif
    }

  public:
    void simulation_phase_callback() {
        // As we are in the systemc thread, if we are chasing another process
        // which locks, that is 'unlocked' we will see the resulting pending activity,
        // so we will not lock here.
        if (!sc_pending_activity()) {
            global_semaphore.wait(); // No pending activity, and the global semaphore
            // has nothing in it - so wait.
            global_semaphore.post(); // replace the token we used.
        }
    }
};
typedef gs_event_async event_async;

class centralSyncPolicy : sc_core::sc_module {
    sc_core::sc_time invalid;
    std::list<sc_core::sc_time> endTimes;
    spin_mutex mutex; // to keep our own data safe
    timed_cond ahead;
    sc_core::sc_time backWindow;
    sc_core::sc_time frontWindow;
    sc_core::sc_time currentDiff;
    sc_core::sc_time rollingAv;

    sem_i canLock;
    int locksToDo;

    /* It is always safe to call this method from anywhere in the SystemC thread */
    void takeLockMethod() {
        mutex.lock();
        for (; locksToDo > 0; locksToDo--) {
            while (sem_trywait(&canLock()) != 0) {
            }
        }
        mutex.unlock();
    }

  public:
    static centralSyncPolicy share;

    void releaseLock() { canLock.post(); }
    void takeLock() {
        mutex.lock();
        locksToDo++;
        checkWindowEvent.notify();
        mutex.unlock();
    }
    void kickLock() {
        mutex.lock();
        canLock.post();
        locksToDo++;
        checkWindowEvent.notify();
        mutex.unlock();
    }

    SC_CTOR(centralSyncPolicy)
        : endTimes(), mutex(), backWindow(SC_ZERO_TIME), frontWindow(SC_ZERO_TIME), currentDiff(SC_ZERO_TIME),
          rollingAv(SC_ZERO_TIME), canLock(0), locksToDo(0) {
        sc_core::sc_set_stop_mode(SC_STOP_IMMEDIATE); // this prevents sc_stop
        // posting an invisible event ! (which causes us to hang)

        SC_METHOD(timePasser);
        sensitive << timePasserEvent;
        dont_initialize();

        SC_METHOD(checkWindow);
        sensitive << checkWindowEvent;
        dont_initialize();
    }
    void start_of_simulation() {
        // need to wait till start of simulation to start the window, because
        // otherwise the quantum may not be set.
        checkWindowEvent.notify(tlm_utils::tlm_quantumkeeper::get_global_quantum());
        COUT << " quantum " << tlm_utils::tlm_quantumkeeper::get_global_quantum() << "\n";
    }

    ~centralSyncPolicy() {
        sc_core::sc_time quantum = tlm_utils::tlm_quantumkeeper::get_global_quantum();

        COUT << "Last time disparity : " << currentDiff.to_seconds() << "s ( " << currentDiff / quantum
             << " quantums ) Rolling Average (100 last time updates) " << rollingAv / quantum << " quantums \n";

        /**
         *  take the lock, before you exit, will force others out of the critical region
         *  (preventing segfault on exit)
         */
        mutex.lock();
    }

    /* thread safe operation */
    void setWindow(sc_core::sc_time t, sc_core::sc_time *entryRef, bool decoupled) {
        mutex.lock();
        *entryRef = t;
        sc_core::sc_time back = sc_time_stamp();
        sc_core::sc_time front = back;

        sc_core::sc_time quantum = tlm_utils::tlm_quantumkeeper::get_global_quantum();

        for (std::list<sc_core::sc_time>::iterator i = endTimes.begin(); i != endTimes.end(); ++i) {
            if (*i < back)
                back = *i;
            if (*i > front)
                front = *i;
        }
        bool windowChanged = (backWindow != back || frontWindow != front);
        backWindow = back;
        frontWindow = front;

        mutex.unlock();

        // just for stats
        currentDiff = (frontWindow - backWindow);
        rollingAv = ((rollingAv * 99) + currentDiff) / 100;
        if (currentDiff / quantum > 2) {
            COUT << " t:" << t << " front:" << frontWindow << " back:" << backWindow << " diff "
                 << currentDiff / quantum << "\n";
        }
        if (windowChanged) {
            ahead.release();
            // here we 'kick' the SystemC window
            kickLock();
        }

        while (!decoupled && (t > backWindow + quantum)) {
            if (ahead.wait()) {
                COUT << "Timeout reached\n";
                break;
            }
        }
    }

    sc_core::sc_time getBackWindow() { return backWindow; }
    sc_core::sc_time getFrontWindow() { return frontWindow; }

    sc_core::sc_time *registerLockable() {
        mutex.lock();
        endTimes.push_back(SC_ZERO_TIME);
        sc_core::sc_time *entryRef = &(endTimes.back());

        mutex.unlock();
        return entryRef;
    }

    sc_core::sc_event timePasserEvent;
    void timePasser() {}

    // we are called when the window changes, or we call ourself
    // Or - critically - we get interupted by a txn, and then the txn
    // finishes, we must be called again....
    // If we are behind, notify ourself in the future, and check again.
    // If we are ahead, sleep till we're woken again.
    void checkWindow() {
        if (sc_time_stamp() < frontWindow) {
            //                checkWindowEvent.notify(tlm_utils::tlm_quantumkeeper::get_global_quantum());
            checkWindowEvent.notify(frontWindow - sc_time_stamp());
        } else {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;

            takeLockMethod(); // Ensure there are no pending token requests.

            if (sem_timedwait(&(canLock()), &ts) == 0) {
                // critical we take the lock here, incase the 'kickLock' is
                // about to post a notify
                mutex.lock();
                sem_post(&(canLock()));
                mutex.unlock();
                // we  rely on checkWindowEvent being notified at the end of the
                // transaction
            } else {
                checkWindowEvent.notify();
            }
        }
    }
    event_async checkWindowEvent;
};

class syncSource {
    sc_core::sc_time *entityRef;
    sc_core::sc_time localBackWindow;
    pthread_t mainThread;
    bool decoupled;

  public:
    syncSource(bool _decoupled = false) : decoupled(_decoupled) {
        mainThread = pthread_self();
        entityRef = centralSyncPolicy::share.registerLockable();

        localBackWindow = SC_ZERO_TIME;
        centralSyncPolicy::share.setWindow(localBackWindow, entityRef, decoupled);
    }

    // CanLock is a semaphore which allows SystemC to lock if SystemC gets
    // ahead.
    void syncAt(sc_core::sc_time t) {
        if (pthread_equal(pthread_self(), mainThread)) {
            return;
        }

        if (t > localBackWindow) {
            localBackWindow = t;
            // setWindow could lock if WE have got ahead (systemC is behind)
            if (decoupled && localBackWindow < sc_time_stamp()) {
                localBackWindow = sc_time_stamp();
            }
            centralSyncPolicy::share.setWindow(localBackWindow, entityRef, decoupled);
        }
    }
};

class decoupledSource : public syncSource {
  public:
    decoupledSource() : syncSource(true) {}
};
}
}

#undef COUT

#endif
