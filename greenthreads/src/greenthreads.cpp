#include "greenthreads/inlinesync.h"

gs::gt::sem_i gs::gt::gs_event_async::global_semaphore(0);

#ifndef SC_HAS_ASYNC_ATTACH_SUSPENDING
gs::gt::before_end_of_delta_helper *gs::gt::gs_event_async::helper = NULL;
#endif

gs::gt::centralSyncPolicy gs::gt::centralSyncPolicy::share("CentralSyncPolicy");
