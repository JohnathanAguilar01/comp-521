# ============================================
# COMP 521 - Assignment 5
# Disk Partition Manager
# ============================================

################################################################
# Function to check correct inputs
function Read-ValidatedInput {
    param (
        [string]$Prompt,
        [scriptblock]$Validator,
        [string]$ErrorMessage
    )

    do {
        $input = Read-Host $Prompt
        $isValid = & $Validator $input

        if (-not $isValid) {
            Write-Host $ErrorMessage -ForegroundColor Red
        }
    } until ($isValid)

    return $input
}

################################################################
# Create new partition
function create_partition {
	param ($ptable)

	# define tab

	# prompt for volume (to be used as hash key)
	# check if drive partition already exists--if so, exit
    Write-Host ""
    $drive = Read-Host "Enter Volume (Drive letter)"
    if ($ptable.ContainsKey($drive)) {
        Write-Host "Partition already exists on drive $drive"
        return
    }
  
	# prompt for other fields (partition type, file system, status, capacity, free space, fault tolerance, overhead)
	# calculate free space percent
	$type = Read-ValidatedInput `
	    "Enter partition type (Reserved,Basic)" `
	    { param($v) $v -in @("Reserved","Basic") } `
	    "Invalid type. Enter Reserved or Basic."

	$fs = Read-ValidatedInput `
	    "Enter file system (NTFS,FAT32)" `
	    { param($v) $v -in @("NTFS","FAT32") } `
	    "Invalid file system. Enter NTFS or FAT32."

	$status = Read-ValidatedInput `
	    "Enter Status (Healthy, Healthy(System), Healthy(Active))" `
	    { param($v) $v -in @("Healthy","Healthy(System)","Healthy(Active)") } `
	    "Invalid status."

	$size = Read-ValidatedInput `
	    "Enter capacity (GB)" `
	    { param($v) $v -as [double] -and $v -gt 0 } `
	    "Capacity must be a positive number."

	$free = Read-ValidatedInput `
	    "Enter free space (GB)" `
	    { param($v) $v -as [double] -and $v -ge 0 } `
	    "Free space must be a number ≥ 0."

	$ft = Read-ValidatedInput `
	    "Enter fault tolerance (Yes/No)" `
	    { param($v) $v -in @("Yes","No") } `
	    "Enter Yes or No."

	$overhead = Read-ValidatedInput `
	    "Enter overhead %" `
	    { param($v) ($v -as [double]) -ne $null -and $v -ge 0 -and $v -le 100 } `
	    "Overhead must be a number between 0 and 100."


	# concatenate field values to be used as value for hash key
	# set hash key value to concatenated fields
    $ptable[$drive] = "$type$tab$fs$tab$status$tab$size$tab$free$tab$ft$tab$overhead"
	
	return $ptable
} # end function

# Resize partition
################################################################
function resize_partition {
	param ($ptable)
	
	# define tab
	
	# prompt for volume (to be used as hash key)
	# check if drive partition does not already exists--if not, exit
    Write-Host ""
    $drive = Read-Host "Enter Volume (Drive letter)"
    if (-not $ptable.ContainsKey($drive)) {
        Write-Host "No partition exists on drive $drive"
        return
    }

	# extract individuals fields from hash key value using split based on tab
    $fields = $ptable[$drive].Split($tab)

	# set individual fields based on respective index
    $type     = $fields[0]
    $fs       = $fields[1]
    $status   = $fields[2]
    $size     = [double]$fields[3]
    $free     = [double]$fields[4]
    $ft       = $fields[5]
    $overhead = $fields[6]	

	# prompt for new capacity value
	    $newSize = Read-ValidatedInput `
        	"Enter new capacity for partition (current value: $size)" `
        	{ param($v) ($v -as [double]) -ne $null -and $v -gt 0 } `
        	"Capacity must be a positive number."
	# recalculate free space & free space percent
	$used = $size - $free
    $newFree = $newSize - $used

    if ($newFree -lt 0) {
        Write-Host "Error: New capacity is smaller than used space." -ForegroundColor Red
        return $ptable
    }
	
	# concatenate field values to be used as value for hash key
	# set hash key value to concatenated fields
	$ptable[$drive] = "$type$tab$fs$tab$status$tab$newSize$tab$newFree$tab$ft$tab$overhead"

	
	return $ptable
} # end function

# ################################################################
# # Delete partition
# function delete_partition {
# param ($ptable)
# 
# # prompt for volume (to be used as hash key)
# # check if drive partition does not already exists--if not, exit
# 
# # Remove key and contents from hash & output message
# 
# return $ptable
# } # end function
# 
# ################################################################
# Print partition table
function list_partitions {
	param ($ptable)

	# define tab

	# Display field names (use -nonewline to keep on same line)
	Write-Host ""
    Write-Host "Volume`tLayout`t`tType`tFS`tStatus`tSize`tFree`t% Free`tFT`tOverhead"
    Write-Host "--------------------------------------------------------------------------------------"

	# for each key in hash, print value (volume)
    foreach ($drive in $ptable.Keys) {
		# Extract individual fields from hash key entry based on splitting on tab	
        $fields = $ptable[$drive].Split($tab)
        $type = $fields[0]
        $fs = $fields[1]
        $status = $fields[2]
        $size = [double]$fields[3]
        $free = [double]$fields[4]
        $ft = $fields[5]
        $over = $fields[6]

		# for each field in the hash value
        $percent = 0
        if ($size -gt 0) { $percent = [math]::Round(($free / $size) * 100) }

		# output field value
        Write-Host "($($drive):)`tPartition`t$type`t$fs`t$status`t$size`t$free`t$percent%`t$ft`t$over"
	} # end for-each
	return $ptable
} # end function

################################################################
# Main script

$tab = [char]9 # define tab
$ptable = @{}  # define empty hash table

# intiialize user choice to empty ""
$choice = ""

# while user does not choose "quit", loop
while ($true) {
	# output menu of options
    Write-Host ""
    Write-Host "Disk Partition Manager"
    Write-Host ""
    Write-Host "create = Create a new partition"
    Write-Host "resize = Resize a partition"
    Write-Host "delete = Delete a partition"
    Write-Host "list   = List existing partitions"
    Write-Host "quit   = Quit"
    Write-Host ""

	# prompt for user choice input
    $choice = Read-Host "Enter choice"

	# switch on user choice to respective function
    switch ($choice) {
        "create" { $ptable = create_partition $ptable }
        "resize" { $ptable = resize_partition $ptable }
        # "delete" { $ptable = Delete-Partition $ptable }
        "list"   { $ptable = list_partitions $ptable }
        # "quit"   { break }
        default { Write-Host "Invalid option" }
    } # end switch
} # end while	

Write-Host ""
Write-Host "Exiting Disk Partition Manager..."
