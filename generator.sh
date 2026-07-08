docker exec -w /src/yesmanchyk/ad 9c38551e45b3 /src/gcc/yesmanchyk/gcc-mirror-install/bin/g++ -std=c++26 -freflection main.cpp -o generator
docker exec -w /src/yesmanchyk/ad 9c38551e45b3 ./generator
