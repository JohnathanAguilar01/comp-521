/* Libraries */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/mutex.h>

/* Parameters: Format: module_param(name, type, default)*/
static int my_size = 0; 
module_param(my_size, int, 0);

/* Array: Format: module_param_array(name, type, size, default for each element)*/
static int my_data[20] = {0}; 
module_param_array(my_data, int, &my_size, 0);

/* Mutex for synchronizing printk calls */
static DEFINE_MUTEX(print_mutex);

/* Thread parameters struct */
struct sort_params {
    int *data;
    int size;
    struct completion done;
};

/*********** Prototype Functions ***********/
void merge(int *left, int *right, int l_end, int r_end);
int sorting_thread(void *params);

/*********** Merge Function ***********/
void merge(int *left, int *right, int l_end, int r_end){
    int l_idx = 0, r_idx = 0, idx = 0;
    int *temp = kmalloc((l_end + r_end) * sizeof(int), GFP_KERNEL);
    int i; 

    if (!temp) {
        printk(KERN_ERR "Failed to allocate memory for merge\n");
        return;
    }

    /* Lock mutex for printing */
    mutex_lock(&print_mutex);
    
    printk(KERN_CONT "Merging: [ ");
    for (i = 0; i < l_end; i = i + 1){
        printk(KERN_CONT "%d ", left[i]);
    }
    printk(KERN_CONT "] and [ ");
    for (i = 0; i < r_end; i = i + 1){
        printk(KERN_CONT "%d ", right[i]);
    }
    printk(KERN_CONT "]\n");
    
    /* Unlock mutex after printing */
    mutex_unlock(&print_mutex);

    while ((l_idx < l_end) && (r_idx < r_end)){
        if (left[l_idx] < right[r_idx]){
            temp[idx++] = left[l_idx++];
        }
        else {
            temp[idx++] = right[r_idx++];
        }
    }
    
    while (l_idx < l_end){
        temp[idx++] = left[l_idx++];
    }
    
    while (r_idx < r_end){
        temp[idx++] = right[r_idx++];
    }
    
    for(idx = 0; idx < (l_end + r_end); idx++){
        if (idx < l_end){ 
            left[idx] = temp[idx];
        }
        else{
            right[idx - l_end] = temp[idx];
        }
    }

    kfree(temp);
}

/************ SORTING THREAD ***********/
int sorting_thread(void *arg){
    struct sort_params *params = (struct sort_params *)arg;
    struct sort_params *left_params = NULL;
    struct sort_params *right_params = NULL;
    struct task_struct *left_thread = NULL;
    struct task_struct *right_thread = NULL;
    int mid;

    if (params->size > 1){
        mid = params->size / 2;
        
        /* Allocate parameters for left thread */
        left_params = kmalloc(sizeof(struct sort_params), GFP_KERNEL);
        if (!left_params) {
            printk(KERN_ERR "Failed to allocate left_params\n");
            complete(&params->done);
            return -ENOMEM;
        }
        left_params->data = params->data;
        left_params->size = mid;
        init_completion(&left_params->done);
        
        /* Allocate parameters for right thread */
        right_params = kmalloc(sizeof(struct sort_params), GFP_KERNEL);
        if (!right_params) {
            printk(KERN_ERR "Failed to allocate right_params\n");
            kfree(left_params);
            complete(&params->done);
            return -ENOMEM;
        }
        right_params->data = params->data + mid;
        right_params->size = params->size - mid;
        init_completion(&right_params->done);

        /* Create and run left sorting thread */
        left_thread = kthread_run(sorting_thread, left_params, "sort_left");
        if (IS_ERR(left_thread)) {
            printk(KERN_ERR "Failed to create left thread\n");
            kfree(left_params);
            kfree(right_params);
            complete(&params->done);
            return PTR_ERR(left_thread);
        }

        /* Create and run right sorting thread */
        right_thread = kthread_run(sorting_thread, right_params, "sort_right");
        if (IS_ERR(right_thread)) {
            printk(KERN_ERR "Failed to create right thread\n");
            wait_for_completion(&left_params->done);
            kfree(left_params);
            kfree(right_params);
            complete(&params->done);
            return PTR_ERR(right_thread);
        }

        /* Wait for both threads to complete */
        wait_for_completion(&left_params->done);
        wait_for_completion(&right_params->done);

        /* Merge the sorted halves */
        merge(params->data, params->data + mid, mid, params->size - mid);

        /* Free allocated memory */
        kfree(left_params);
        kfree(right_params);
    }
    
    /* Signal completion */
    complete(&params->done);
    return 0;
}

/* This function is called when module is loaded */
static int __init proc_init(void){
    struct sort_params *my_array;
    struct task_struct *main_thread;
    int i;
    
    /* Allocate memory for sort_params struct */
    my_array = kmalloc(sizeof(struct sort_params), GFP_KERNEL);
    if (!my_array) {
        printk(KERN_ERR "Failed to allocate memory\n");
        return -ENOMEM;
    }
    
    /* Allocate and copy data array */
    my_array->data = kmalloc(my_size * sizeof(int), GFP_KERNEL);
    if (!my_array->data) {
        kfree(my_array);
        printk(KERN_ERR "Failed to allocate memory for data\n");
        return -ENOMEM;
    }
    
    /* Copy module parameter data to allocated array */
    for (i = 0; i < my_size; i++) {
        my_array->data[i] = my_data[i];
    }
    my_array->size = my_size;
    init_completion(&my_array->done);

    /* Verify user input was correct */
    printk(KERN_INFO "Size of list: %d\n", my_array->size);
    printk(KERN_CONT "Original list: [ ");
    for (i = 0; i < my_array->size; i = i + 1){
        printk(KERN_CONT "%d ", my_array->data[i]);
    }
    printk(KERN_CONT "]\n");

    /* Create and run the main sorting thread */
    main_thread = kthread_run(sorting_thread, my_array, "sort_main");
    if (IS_ERR(main_thread)) {
        printk(KERN_ERR "Failed to create main thread\n");
        kfree(my_array->data);
        kfree(my_array);
        return PTR_ERR(main_thread);
    }

    /* Wait for sorting to complete */
    wait_for_completion(&my_array->done);

    /* Print sorted list */
    printk(KERN_CONT "Sorted list: [ ");
    for (i = 0; i < my_array->size; i = i + 1){
        printk(KERN_CONT "%d ", my_array->data[i]);
    }
    printk(KERN_CONT "]\n");

    /* Free allocated memory */
    kfree(my_array->data);
    kfree(my_array);

    return 0; 
}

/* This function is called when module is removed */
static void __exit proc_exit(void){
    printk(KERN_INFO "Merge Sort module unloaded\n");
}

/* Module Declarations */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Multi-threaded Merge Sort with kernel threads");
MODULE_AUTHOR("YERAZIK");
