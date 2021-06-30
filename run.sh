echo "Compiling..."
g++ lodepng.cpp create_image.cpp -lpthread
echo "Generating image..."
./a.out
echo "Opening image..."
xdg-open image.png > /dev/null
