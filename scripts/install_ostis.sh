#!/bin/bash

prepare_platform()
{
		cd scripts
		sudo apt-get update
		sudo apt-get install nodejs-dev node-gyp libssl1.0-dev
		./prepare.sh
		cd ..
}

include_kb()
{
  rm ./ims.ostis.kb/ui/ui_start_sc_element.scs
  rm -rf ./kb/menu
  echo "../kb" >> ./repo.path
  echo "../problem-solver" >> ./repo.path
  cd scripts
  ./build_kb.sh
  cd ..
}

include_kpm()
{
  cd sc-machine
  echo 'add_subdirectory(${SC_MACHINE_ROOT}/../../problem-solver/cxx ${SC_MACHINE_ROOT}/bin)' >> ./CMakeLists.txt
	cd ./scripts
	./make_all.sh
	cd ../..
}

cd ..
if [ -d "ostis" ]; 
	then
		echo -en "Update OSTIS platform\n"
		cd ostis
		git pull
		prepare_platform
	else
		echo -en "Install OSTIS platform\n"
		git clone https://github.com/ShunkevichDV/ostis.git
		cd ostis
    git checkout 0.5.0
		prepare_platform
		include_kb
		include_kpm
fi

