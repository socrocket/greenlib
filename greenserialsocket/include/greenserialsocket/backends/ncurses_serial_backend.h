#include <greenserialsocket/tlm_serial_bidirectional_socket.h>
#include <greenthreads/thread_safe_event.h>

#include <ncurses.h>
#include <queue>

class StdioSerial : sc_module
{
protected:
    gs::gt::gs_event_async m_async_event;

    std::queue<unsigned char> m_queue;

public:
    tlm_serial::tlm_serial_bidirectional_socket<> socket;

    SC_HAS_PROCESS(StdioSerial);

    StdioSerial(sc_module_name name)
        : sc_module(name)
        , m_async_event("event")
        , socket("socket")
    {
        socket.register_b_transport(this, &StdioSerial::b_transport);

        SC_METHOD(rcv);
        sensitive << m_async_event;
        dont_initialize();

        pthread_t async_thread;
        if (pthread_create(&async_thread, NULL, rcv_thread, this)) {
            fprintf(stderr, "error creating thread\n");
        }
    }

    static void *rcv_thread(void *arg)
    {
        StdioSerial *serial = (StdioSerial *)arg;

        initscr();
        noecho();
        cbreak();
        timeout(100);

        for (;;) {
            int c = getch();
            if (c >= 0) {
                serial->m_queue.push(c);
            }
            if (!serial->m_queue.empty()) {
                serial->m_async_event.notify();
            }
        }
    }

    void rcv(void)
    {
        sc_core::sc_time delay = sc_core::sc_time(sc_core::SC_ZERO_TIME);
        tlm_serial::tlm_serial_payload *txn = socket.get_transaction();

        unsigned char c;
        while (!m_queue.empty()) {
            c = m_queue.front();
            m_queue.pop();
            txn->set_command(tlm_serial::SERIAL_SEND_CHAR_COMMAND);
            txn->set_data_ptr(&c);
            txn->set_data_length(1);
            txn->set_response_status(tlm_serial::SERIAL_OK_RESPONSE);
            txn->set_num_stop_bits_in_end(1);
            txn->set_enable_bits(tlm_serial::SERIAL_STOP_BITS_ENABLED);
            socket->b_transport(*txn, delay);
        }

        txn->release();
    }

    void b_transport(tlm_serial::tlm_serial_payload &gp, sc_core::sc_time &time)
    {
        unsigned char *data = gp.get_data_ptr();
        int len = gp.get_data_length();

        for (int i = 0; i < len; i++) {
            putchar(data[i]);
            fflush(stdout);
        }
    }
};


