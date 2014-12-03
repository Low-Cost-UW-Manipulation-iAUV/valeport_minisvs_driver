#!/bin/bash
echo "I'll copy all ADIS_Interface files over to the BBB"

rsync -avzh CMakeLists.txt 						bmt:/home/devel/catkin_ws/src/uwesub_svs/
rsync -avzh *.xml 								bmt:/home/devel/catkin_ws/src/uwesub_svs/
rsync -avzh ./src/*.cpp 						bmt:/home/devel/catkin_ws/src/uwesub_svs/src/
rsync -avzh ./include/uwesub_svs/*.hpp 			bmt:/home/devel/catkin_ws/src/uwesub_svs/include/uwesub_svs/
rsync -avzh *.launch	 						bmt:/home/devel/catkin_ws/src/uwesub_svs/
rsync -avzh *.yaml		 						bmt:/home/devel/catkin_ws/src/uwesub_svs/
rsync -avzh *.md								bmt:/home/devel/catkin_ws/src/uwesub_svs/
rsync -avzh ./urdf/*.urdf						bmt:/home/devel/catkin_ws/src/uwesub_svs/urdf/

echo "All done, Good Success!"