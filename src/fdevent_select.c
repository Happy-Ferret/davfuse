#include <errno.h>
#include <sys/select.h>

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "c_util.h"
#include "events.h"
#include "logging.h"
#include "fdevent_select.h"

NON_NULL_ARGS0() bool
fdevent_init(FDEventLoop *loop) {
  assert(loop);
  loop->ll = NULL;
  return true;
}

NON_NULL_ARGS2(1, 4) bool
fdevent_add_watch(FDEventLoop *loop,
                  int fd,
                  StreamEvents events,
                  event_handler_t handler,
                  void *ud,
                  fd_event_watch_key_t *key) {
  FDEventLink *ew;

  assert(loop);
  assert(handler);

  ew = malloc(sizeof(*ew));
  if (!ew) {
    *key = FD_EVENT_INVALID_WATCH_KEY;
    return false;
  }

  *ew = (FDEventLink) {
    .ew = {fd, ud, events, handler},
    .prev = NULL,
    .next = NULL,
    .active = true,
  };

  if (loop->ll) {
    ew->next = loop->ll;
    loop->ll->prev = ew;
    loop->ll = ew;
  }
  else {
    loop->ll = ew;
  }

  if (key) {
    *key = ew;
  }

  return true;
}

NON_NULL_ARGS0() bool
fdevent_remove_watch(FDEventLoop *loop,
                     fd_event_watch_key_t key) {
  /* fd_event_watch_key_t types are actually pointers to FDEventLink types */
  FDEventLink *ll = key;

  assert(loop);
  assert(loop->ll);
  assert(key);

  ll->active = false;

  return true;
}

static void
_actually_free_link(FDEventLoop *loop, FDEventLink *ll) {
  if (ll->prev) {
    ll->prev->next = ll->next;
  }

  if (ll->next) {
    ll->next->prev = ll->prev;
  }

  if (ll == loop->ll) {
    assert(!ll->prev);
    loop->ll = ll->next;
  }

  free(ll);
}

bool
fdevent_main_loop(FDEventLoop *loop) {

  while (true) {
    fd_set readfds, writefds;
    int nfds = -1;
    FDEventLink *ll = loop->ll;

    log_info("Looping...");

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    while (ll) {
      if (!ll->active) {
        FDEventLink *tmpll = ll->next;
        _actually_free_link(loop, ll);
        ll = tmpll;
        continue;
      }

      if (ll->ew.events.read) {
        log_debug("Adding fd %d to read set", ll->ew.fd);
	FD_SET(ll->ew.fd, &readfds);
      }

      if (ll->ew.events.write) {
        log_debug("Adding fd %d to write set", ll->ew.fd);
	FD_SET(ll->ew.fd, &writefds);
      }

      if (ll->ew.fd > nfds) {
	nfds = ll->ew.fd;
      }

      ll = ll->next;
    }

    /* if there is nothing to select for, then stop the main loop */
    if (nfds < 0) {
      return true;
    }

    while (select(nfds + 1, &readfds, &writefds, NULL, NULL) < 0) {
      if (errno != EINTR) {
        log_error_errno("Error while doing select()");
        return false;
      }
    }

    /* now dispatch on events */
    ll = loop->ll;
    while (ll) {
      StreamEvents events;

      if (ll->active) {
        events = create_stream_events(FD_ISSET(ll->ew.fd, &readfds),
                                      FD_ISSET(ll->ew.fd, &writefds));
        if ((events.read && ll->ew.events.read) ||
            (events.write && ll->ew.events.write)) {
          event_handler_t h = ll->ew.handler;
          int fd = ll->ew.fd;
          void *ud = ll->ew.ud;
          FDEvent e = (FDEvent) {
            .loop = loop,
            .fd = fd,
            .events = events,
          };
          fdevent_remove_watch(loop, ll);
          h(FD_EVENT, &e, ud);
        }
      }

      ll = ll->next;
    }
  }
}
