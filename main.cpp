/**
 * Author: Andrei Cioara <http://andrei.cioara.me>
 *
 * Credits
 * http://unix.stackexchange.com/questions/94322/is-it-possible-for-a-daemon-i-e-background-process-to-look-for-key-presses-fr
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include <string.h>     /* strerror() */
#include <errno.h>      /* errno */

#include <fcntl.h>      /* open() */
#include <unistd.h>     /* close() */
#include <sys/ioctl.h>  /* ioctl() */

#include <linux/input.h>    /* EVIOCGVERSION ++ */

#define EV_BUF_SIZE 16

#include <iostream>
using namespace std;

int fd;
int debug;

void print_debug() {
  /* A few examples of information to gather */
  unsigned version;
  unsigned short id[4];                   /* or use struct input_id */
  char name[256] = "N/A";

  /* Error checks */
  ioctl(fd, EVIOCGVERSION, &version);
  ioctl(fd, EVIOCGID, id);
  ioctl(fd, EVIOCGNAME(sizeof(name)), name);

  fprintf(
      stderr,
      "Name      : %s\n"
          "Version   : %d.%d.%d\n"
          "ID        : Bus=%04x Vendor=%04x Product=%04x Version=%04x\n"
          "----------\n",
      name,
      version >> 16,
      (version >> 8) & 0xff,
      version & 0xff,

      id[ID_BUS],
      id[ID_VENDOR],
      id[ID_PRODUCT],
      id[ID_VERSION]
  );
}

int main(int argc, char *argv[]) {
  debug = 0;

  if (argc < 2) {
    fprintf(
        stderr,
        "Usage: %s /dev/input/event3 [callbackProgram.exe]",
        argv[0]
    );
    return EINVAL;
  }

  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    fprintf(
        stderr,
        "ERR %d:\n Unable to open `%s'\n %s\n",
        errno, argv[1], strerror(errno)
    );
  }

  if (debug) {
    print_debug();
  }

  struct input_event event_buffer[EV_BUF_SIZE];
  for (; ;) {
    ssize_t sz = read(fd, event_buffer, sizeof(struct input_event) * EV_BUF_SIZE);

    if (sz < (int) sizeof(struct input_event)) {
      fprintf(
          stderr,
          "ERR %d:\n"
              "Reading of `%s' failed\n"
              "%s\n",
          errno, argv[1], strerror(errno)
      );
    }

    break;

    /* Implement code to translate type, code and value */
    for (int i = 0; i < sz / sizeof(struct input_event); ++i) {
      fprintf(
          stderr,
          "%ld.%06ld: "
              "type=%02x "
              "code=%02x "
              "value=%02x\n",
          event_buffer[i].time.tv_sec,
          event_buffer[i].time.tv_usec,
          event_buffer[i].type,
          event_buffer[i].code,
          event_buffer[i].value
      );
    }
  }

  uid_t ruid = getuid(); // Real ID
  uid_t euid = geteuid(); // Effective ID
  seteuid(ruid); // Dropping root privileges

  string callbackCommand = "";

  for (int i = 2; i < argc; i++) {
    callbackCommand += argv[i];
    if (i != argc - 1) {
      callbackCommand += " ";
    }
  }

  if (callbackCommand.size()) {
    system(callbackCommand.c_str());
  }
}
