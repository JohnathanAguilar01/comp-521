################################################################
# Create new partition
function create_partition {
param ($ptable)

# define tab

# prompt for volume (to be used as hash key)
# check if drive partition already exists--if so, exit
  
# prompt for other fields (partition type, file system, status, capacity, free space, fault tolerance, overhead)
# calculate free space percent

# concatenate field values to be used as value for hash key
# set hash key value to concatenated fields
  
return $ptable
} # end function

################################################################
# Resize partition
function resize_partition {
param ($ptable)

# define tab

# prompt for volume (to be used as hash key)
# check if drive partition does not already exists--if not, exit

# extract individuals fields from hash key value using split based on tab
# set individual fields based on respective index
# prompt for new capacity value
# recalculate free space & free space percent

# concatenate field values to be used as value for hash key
# set hash key value to concatenated fields
  
return $ptable
} # end function

################################################################
# Delete partition
function delete_partition {
param ($ptable)

# prompt for volume (to be used as hash key)
# check if drive partition does not already exists--if not, exit

# Remove key and contents from hash & output message

return $ptable
} # end function

################################################################
# Print partition table
function list_partitions {
param ($ptable)

# define tab

# Display field names (use -nonewline to keep on same line)

# for each key in hash, print value (volume)
	# Extract individual fields from hash key entry based on splitting on tab	
	
	# for each field in the hash value
		# output field value
} # end for-each
return $ptable
} # end function

################################################################
# Main script

# define empty hash table
# define tab

# intiialize user choice to empty ""

# while user does not choose "quit", loop
	# output menu of options

	# prompt for user choice input

	# switch on user choice to respective function
		(Optional) output message if illegal choice
	} # end switch
} # end while	