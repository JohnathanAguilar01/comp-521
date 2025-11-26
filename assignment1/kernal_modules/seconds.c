#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timekeeping.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"

static time64_t start_seconds;

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static const struct proc_ops proc_ops = {
    .proc_read = proc_read,
};

static int __init seconds_init(void){
  start_seconds = ktime_get_real_seconds();
  proc_create(PROC_NAME, 0, NULL, &proc_ops);
  printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
  return 0;
}

static void __exit seconds_exit(void){
  remove_proc_entry(PROC_NAME, NULL);
  printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos){
  int rv = 0;
  char buffer[BUFFER_SIZE];
  static int completed = 0;
  time64_t now = ktime_get_real_seconds();
  time64_t elapsed_seconds = now - start_seconds;
  
  if (completed) {
    completed = 0;
    return 0;
  }
  
  completed = 1;
  
  rv = sprintf(buffer, "Elapsed seconds: %lld\n", elapsed_seconds);
  
  // copies the contents of buffer to userspace usr_buf
  long bytes_not_copied = copy_to_user(usr_buf, buffer, rv);
  
  if (bytes_not_copied == 0) {
      return rv;  // all bytes copied successfully
  } else {
      return rv - bytes_not_copied;  // partial copy
  }
}

module_init(seconds_init);
module_exit(seconds_exit);

MODULE_AUTHOR("Johnathan Aguilar");
MODULE_DESCRIPTION("seconds module");
MODULE_LICENSE("GPL"); // important: avoid taint warnings
