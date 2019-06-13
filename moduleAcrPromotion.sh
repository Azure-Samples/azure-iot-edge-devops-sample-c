#!/bin/bash

# Modules(s) ACR Promotion Script 
# Requires az cli

usage(){
        echo "***Module ACR Promotion Script***"
        echo "Usage: ./moduleAcrPromotion.sh <src_acr_url> <dest_acr_name> <module_name1:tag> <module_name2:tag> <module_nameN:tag>"
		echo "Expects at least 3 arguments: src_acr_url, dest_acr_name, and at least 1 module to promote"
}

promoteModules(){
for ((i=3;i<=$#;i++))
do
  echo "Promoting ${!i} from $src_acr to $dest_acr..."
  az acr import -n $dest_acr -t ${!i} --source $src_acr/${!i}
done

}

src_acr=$1
dest_acr=$2

#check arguments
[[ $# < 3 ]] && { usage && exit 1; } || promoteModules "$@"