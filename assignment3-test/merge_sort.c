/* Libraries */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>

/* Parameters: Format: module_param(name, type, default)*/
static int my_size = 0; 
module_param(my_size, int, 0);

/* Array: Format: module_param_array(name, type, size, default for each element)*/
static int my_data[20] = {0}; 
module_param_array(my_data, int, &my_size, 0);

/* Thread parameters struct */
struct sort_params {
    int *data;
    int size;
};

/*********** Prototype Functions ***********/
void merge(int *left, int *right, int l_end, int r_end);
int sorting_thread(struct sort_params *params);
int merging_thread(struct sort_params *params);

/*********** Merge Function ***********/
void merge(int *left, int *right, int l_end, int r_end){
    int l_idx = 0, r_idx = 0, idx = 0;
    int *temp = kmalloc((l_end + r_end) * sizeof(int), GFP_KERNEL);
    int i; 

    if (!temp) {
        printk(KERN_ERR "Failed to allocate memory for merge\n");
        return;
    }

    printk(KERN_CONT "Merging: [ ");
    for (i = 0; i < l_end; i = i + 1){
        printk(KERN_CONT "%d ", left[i]);
    }
    printk(KERN_CONT "] and [ ");
    for (i = 0; i < r_end; i = i + 1){
        printk(KERN_CONT "%d ", right[i]);
    }
    printk(KERN_CONT "]\n");

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

/************ SORTING ***********/
int sorting_thread(struct sort_params *params){
    if (params->size > 1){
        int mid = params->size / 2;
        struct sort_params left = {params->data, mid}; 
        struct sort_params right = {params->data + mid, params->size - mid};

        sorting_thread(&left);
        sorting_thread(&right);
        merge(params->data, params->data + mid, mid, params->size - mid);
    }
    return 0;
}

/************ MERGING ***********/
int merging_thread(struct sort_params *params){
    int mid = params->size / 2;
    merge(params->data, params->data + mid, mid, params->size - mid);
    return 0;
}

/* This function is called when module is loaded */
static int __init proc_init(void){
    struct sort_params *my_array;
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

    /* Verify user input was correct */
    printk(KERN_INFO "Size of list: %d\n", my_array->size);
    printk(KERN_CONT "Original list: [ ");
    for (i = 0; i < my_array->size; i = i + 1){
        printk(KERN_CONT "%d ", my_array->data[i]);
    }
    printk(KERN_CONT "]\n");

    /* Perform sorting */
    sorting_thread(my_array);

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
MODULE_DESCRIPTION("Merge Sort with kernel modules");
MODULE_AUTHOR("YERAZIK");
