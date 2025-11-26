#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "jiffies"

static unsigned long start_jiffies;

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static const struct proc_ops proc_ops = {
    .proc_read = proc_read,
};

static int __init jiffies_init(void){
  start_jiffies = jiffies;
  proc_create(PROC_NAME, 0, NULL, &proc_ops);
  printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
  return 0;
}

static void __exit jiffies_exit(void){
  remove_proc_entry(PROC_NAME, NULL);
  printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos){
  int rv = 0;
  char buffer[BUFFER_SIZE];
  static int completed = 0;
  unsigned long elapsed_jiffies = jiffies;
  
  if (completed) {
    completed = 0;
    return 0;
  }
  
  completed = 1;
  
  rv = sprintf(buffer, "Elapsed Jiffies: %lu\n", elapsed_jiffies);
  
  // copies the contents of buffer to userspace usr_buf
  long bytes_not_copied = copy_to_user(usr_buf, buffer, rv);
  
  if (bytes_not_copied == 0) {
      return rv;  // all bytes copied successfully
  } else {
      return rv - bytes_not_copied;  // partial copy
  }
}

module_init(jiffies_init);
module_exit(jiffies_exit);

MODULE_AUTHOR("Johnathan Aguilar");
MODULE_DESCRIPTION("jiffies module");
MODULE_LICENSE("GPL"); // important: avoid taint warnings
