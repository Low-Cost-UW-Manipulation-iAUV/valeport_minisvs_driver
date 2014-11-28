#!/bin/bash
echo "I'll copy all ADIS_Interface files over to the BBB"

rsync -avzh CMakeLists.txt 						eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/
rsync -avzh *.xml 								eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/
rsync -avzh ./src/*.cpp 						eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/src/
rsync -avzh ./include/uwesub_svs/*.hpp 			eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/include/uwesub_svs/
rsync -avzh *.launch	 						eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/
rsync -avzh *.yaml		 						eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/
rsync -avzh *.md								eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/
rsync -avzh ./urdf/*.urdf						eurathlon_vm:/home/euratlhon/uwesub_msc/src/uwesub_svs/urdf/

echo "All done, Good Success!"