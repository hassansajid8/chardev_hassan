#include <asm-generic/errno-base.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/wait.h>

#define SET_SIZE_CALL _IOW('a', 'a', int *)
#define WRITE_CALL _IOW('a', 'b', struct Data)
#define READ_CALL _IOR('a', 'c', struct Data)

// Queue impl

struct Data {
      int length;
      char data[256];
};

struct Queue {
      struct Data *arr;
      int front;
      int rear;
      int size;
      int capacity;
};

// init Queue
static struct Queue *init_queue(int capacity) {
      struct Queue *q =
          (struct Queue *)kmalloc(sizeof(struct Queue), GFP_KERNEL);
      if (!q)
            return NULL;

      q->arr =
          (struct Data *)kmalloc(sizeof(struct Data) * capacity, GFP_KERNEL);
      if (!q->arr) {
            kfree(q);
            return NULL;
      }

      q->front = -1;
      q->rear = -1;
      q->size = 0;
      q->capacity = capacity;

      return q;
}

// resize Queue
static int resize_queue(struct Queue *q) {
      int new_capacity = q->capacity * 2;
      struct Data *new_arr = (struct Data *)kmalloc(
          sizeof(struct Data) * new_capacity, GFP_KERNEL);
      if (!new_arr)
            return -1;

      // copy into new arr
      int i;
      for (i = 0; i < q->size; i++) {
            new_arr[i] = q->arr[(q->front + i) % q->capacity];
      }

      kfree(q->arr);
      q->arr = new_arr;
      q->capacity = new_capacity;
      q->front = 0;
      q->rear = q->size - 1;

      return 0;
}

// enqueue
static int enqueue(struct Queue *q, int datalen, char *data) {
      if (q->size == q->capacity) {
            if (resize_queue(q) == -1) {
                  pr_err("hassan - error resizing queue\n");
                  return -1;
            }
      }

      if (q->front == -1)
            q->front = 0;
      q->rear = (q->rear + 1) % q->capacity;

      q->arr[q->rear].length = datalen;
      strncpy(q->arr[q->rear].data, data, sizeof(q->arr[q->rear].data) - 1);
      q->arr[q->rear].data[sizeof(q->arr[q->rear].data) - 1] = '\0';

      q->size++;
      return 0;
}

// dequeue
static struct Data dequeue(struct Queue *q) {
      if (q->size == 0) {
            struct Data d = {-1, ""};
            return d;
      }

      struct Data d = q->arr[q->front];
      q->front = (q->front + 1) % q->capacity;
      q->size--;

      if (q->size == 0) {
            q->front = -1;
            q->rear = -1;
      }

      return d;
}

// free queue
static void free_queue(struct Queue *q) {
      kfree(q->arr);
      kfree(q);
}

// ------------------------------------------

static int major;
wait_queue_head_t wq;

static DEFINE_MUTEX(q_lock);
static struct Queue *q;

static long ioctl_fn(struct file *fp, unsigned int cmd, unsigned long arg) {
      switch (cmd) {
      case SET_SIZE_CALL:
            {
            pr_info("hassan - SET_SIZE CALL recieved\n");
            int capacity;
            if (copy_from_user(&capacity, (int __user *)arg, sizeof(int))) {
                  return -EFAULT;
            }

            mutex_lock(&q_lock);
            if (!q) {
                  q = init_queue(capacity);
                  if(!q){
                        mutex_unlock(&q_lock);
                        pr_err("hassan - error initializing queue\n");
                        return -ENOMEM;
                  }                        

                  pr_info("hassan - Queue initialized with capacity %d\n",
                          capacity);
                  mutex_unlock(&q_lock);
                  return 0;
            }

            pr_warn("hassan - Queue already initialized with capacity %d and "
                    "has %d elements\n",
                    q->capacity, q->size);
            mutex_unlock(&q_lock);
            return 0;
            }

      case WRITE_CALL:
            {
            pr_info("hassan - WRITE_CALL received\n");
            struct Data d;
            if (copy_from_user(&d, (struct Data __user *)arg,
                               sizeof(struct Data))) {
                  return -EFAULT;
            }
            
            if (d.length >= 256 || d.length <= 0) {
                  return -EINVAL;
            }

            mutex_lock(&q_lock);
            if(enqueue(q, d.length, d.data) == -1){
                  mutex_unlock(&q_lock);
                  pr_err("hassan - error enqueueing data\n");
                  return -EFAULT;
            }

            pr_info("hassan - data enqueued\n");
            mutex_unlock(&q_lock);
            wake_up_interruptible(&wq);
            return 0;
            }

      case READ_CALL:
            {
            pr_info("hassan - READ_CALL \n");

            int wqstatus = wait_event_interruptible(wq, q && q->size > 0);
            if (wqstatus == -ERESTARTSYS) {
                  return wqstatus;
            }

            mutex_lock(&q_lock);

            struct Data x = dequeue(q);

            if (copy_to_user((struct Data __user *)arg, &x,
                             sizeof(struct Data))) {
                  mutex_unlock(&q_lock);
                  return -ENOMEM;
            }

            pr_info("hassan - dequeued data copied to userspace\n");
            mutex_unlock(&q_lock);
            return 0;
            }

      default:
            return -ENOTTY;
      }

      return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = ioctl_fn,
};

static int __init dev_init(void) {
      major = register_chrdev(0, "hassan", &fops);
      if (major < 0) {
            pr_err("hassan - error registering device\n");
            return major;
      }

      pr_info("hassan - device registered with major number %d\n", major);

      init_waitqueue_head(&wq);

      return 0;
}

static void __exit dev_exit(void) {
      unregister_chrdev(major, "hassan");
      pr_info("hassan - device unregistered\n");

      mutex_lock(&q_lock);
      if (q){
            free_queue(q);
            q = NULL;
      }
      mutex_unlock(&q_lock);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hassan");
MODULE_DESCRIPTION("Sample char device driver");
