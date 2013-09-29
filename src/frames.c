/*
 * This file is part of libwebsock
 *
 * Copyright (C) 2012 Payden Sutherland
 *
 * libwebsock is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * libwebsock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libwebsock; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "websock.h"

void
libwebsock_free_all_frames(libwebsock_client_state *state)
{
  libwebsock_frame *current, *next;
  if (state != NULL) {
    current = state->current_frame;
    if (current) {
      for (; current->prev_frame != NULL; current = current->prev_frame);
      while (current != NULL) {
        next = current->next_frame;
        if (current->rawdata) {
          free(current->rawdata);
        }
        free(current);
        current = next;
      }
    }
  }
}

void
libwebsock_handle_control_frame(libwebsock_client_state *state, libwebsock_frame *ctl_frame)
{
  state->control_callback(state, ctl_frame);
  // Control frames can be injected in the midst of a fragmented message.
  // We need to maintain the link to previous frame if present.
  // It should be noted that ctl_frame is still state->current_frame after this function returns.
  // So even though the below refers to ctl_frame, I'm really setting up state->current_frame to continue receiving data on the next go 'round

  //should be able to reuse this frame by setting these two members to zero.  Avoid free/malloc of rawdata
  ctl_frame->state = 0;
  ctl_frame->rawdata_idx = 0;
}

void
libwebsock_cleanup_frames(libwebsock_frame *first)
{
  libwebsock_frame *this = NULL;
  libwebsock_frame *next = first;
  while (next != NULL) {
    this = next;
    next = this->next_frame;
    if (this->rawdata != NULL) {
      free(this->rawdata);
    }
    free(this);
  }
}
