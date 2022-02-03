#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

double RANDOM_COLORS[7][3] = {
	{0.0000,    0.4470,    0.7410},
	{0.8500,    0.3250,    0.0980},
	{0.9290,    0.6940,    0.1250},
	{0.4940,    0.1840,    0.5560},
	{0.4660,    0.6740,    0.1880},
	{0.3010,    0.7450,    0.9330},
	{0.6350,    0.0780,    0.1840},
};
struct Point
{
	Point()
	{
	}
	Point(int a, int b)
	{
		x = a;
		y = b;
	}
	Point(int x, int y, int r, int g, int b)
	{
		this->x = x;
		this->y = y;
		this->r = r;
		this->g = g;
		this->b = b;
	}

	float x;
	float y;
	int r;
	int g;
	int b;
};

struct Triangle
{
	int nMax(int x, int y, int z) { int a = max(x, y); a = max(a, z); return a; }
	int nMin(int x, int y, int z) { int a = min(x, y); a = min(a, z); return a; }

	Triangle(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		v[0] = x1;
		v[1] = y1;
		v[2] = x2;
		v[3] = y2;
		v[4] = x3;
		v[5] = y3;
	}

	Triangle(Point x, Point y, Point z)
	{
		v[0] = x.x;
		v[1] = x.y;
		v[2] = y.x;
		v[3] = y.y;
		v[4] = z.x;
		v[5] = z.y;
		v1 = x;
		v2 = y;
		v3 = z;

		//box.xmin = nMin(v[0], v[2], v[4]);
		//box.xmax = nMax(v[0], v[2], v[4]);
		//box.ymin = nMin(v[1], v[3], v[5]);
		//box.ymax = nMax(v[1], v[3], v[5]);
	}

	float v[6];
	Point v1;
	Point v2;
	Point v3;

};

float distanceFormula(Point p1, Point p2)
{
	float dist = sqrt(abs((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y)));
	return dist;
}
float area(Point p1, Point p2, Point p3)
{
	float s1 = distanceFormula(p1, p2);
	float s2 = distanceFormula(p1, p3);
	float s3 = distanceFormula(p2, p3);

	// Area = sqrt s* (s-a)* (s-b)* (s-c)
	float s = (s1 + s2 + s3) / 2;
	float areaT = sqrt(s * (s - s1) * (s - s2) * (s - s3));
	return areaT;
}
void Barycentric(Point a, Point b, Point c, float& u, float& v, float& w, Point p)
{
	//area = std::abs(x0 * (y1 - y2) + x1 * (y2 - y0) + x2 * (y0 - y1)) / 2
		//areaA = (P, B, C) 
	float areaA = area(p, b, c);
	float areaB = area(a, p, c);
	float areaC = area(a, b, p);
	float totalArea = std::abs(a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2;

	u = (areaA / totalArea);
	v = (areaB / totalArea);
	w = (areaC / totalArea);

}

struct BoundingBox
{
	float nMax(int x, int y, int z) { int a = max(x, y); a = max(a, z); return a; }
	float nMin(int x, int y, int z) { int a = min(x, y); a = min(a, z); return a; }

	BoundingBox() {}

	BoundingBox(Triangle tri)
	{
		xmin = nMin(tri.v[0], tri.v[2], tri.v[4]);
		xmax = nMax(tri.v[0], tri.v[2], tri.v[4]);
		ymin = nMin(tri.v[1], tri.v[3], tri.v[5]);
		ymax = nMax(tri.v[1], tri.v[3], tri.v[5]);
	}
	int xmin;
	int xmax;
	int ymin;
	int ymax;
};

void minMaxs(std::vector<float> posBuf, float &xmin, float& xmax, float& ymin, float& ymax)
{
	int x = 0;
	int index = 0;
	for (int i = 0; i < posBuf.size(); i++)
	{

		if (x % 3 == 0 && x!=0) {}
		else if (x % 2 == 0)
		{
			if (xmin > posBuf.at(i)) { xmin = posBuf.at(i); }
			if (xmax < posBuf.at(i)) { xmax = posBuf.at(i); }
		}
		else
		{
			if (ymin > posBuf.at(i)) { ymin = posBuf.at(i);}
			if (ymax < posBuf.at(i)) { ymax = posBuf.at(i); index = i;}
		}
		x += 1;
	}
	cout << "max index is " << index << "\n";
}

float findScale(std::vector<float> &posBuf, float imageWidth, float imageHeight)
{
	float xmin = posBuf.at(0);
	float xmax = posBuf.at(0);
	float ymin = posBuf.at(1);
	float ymax = posBuf.at(1);
	//Find min maxes of posbuf
	
	minMaxs(posBuf, xmin,xmax,ymin,ymax);

	float scaleX = imageWidth / ((xmax - xmin));
	float scaleY = imageHeight / ((ymax - ymin));
	cout << "SCALE = " << min(scaleX, scaleY) << "\n";
	return (min(scaleX, scaleY));
}

/*
void translateToMid(std::vector<float>& posBuf, float width, float height)
{
	float midX;
	float midY;
	float xmin = posBuf.at(0);
	float xmax = posBuf.at(0);
	float ymin = posBuf.at(1);
	float ymax = posBuf.at(1);

	minMaxs(posBuf, xmin, xmax, ymin, ymax);

	midX = (xmax + xmin) / 2;
	midY = (ymax + ymin) / 2;
	float MidImgX = width / 2;
	float MidImgY = height / 2;

	float transX = MidImgX - midX;
	float transY = MidImgY - midY;

	int x = 0;
	for (int i = 0; i < posBuf.size(); i++)
	{
		if (x % 3 == 0 && x != 0) {}
		else if (x % 2 == 0)
		{
			posBuf.at(i) = posBuf.at(i) + transX;
		}
		else
		{
			posBuf.at(i) = posBuf.at(i) + transY;
		}
		x += 1;
	}
	
}*/
void scaleBuffer(std::vector<float> &posBuf, float width, float height)
{
	float xmin = posBuf.at(0);
	float xmax = posBuf.at(0);
	float ymin = posBuf.at(1);
	float ymax = posBuf.at(1);
	minMaxs(posBuf, xmin, xmax, ymin, ymax);

	cout << "yMAX-yMIN IS " << ymax << "\n";

	float scaleX = width / ((xmax - xmin));
	float scaleY = height / ((ymax - ymin));
	//  1000/()
	float minScale = min(scaleX, scaleY);

	for (int i = 0; i < posBuf.size(); i++)
	{
		posBuf.at(i) = posBuf.at(i) * minScale;
	}

	cout << minScale << "<-- SCALE VALUE\n";
	
}

std::vector<int> findTrans(std::vector<float> posBuf, float width, float height, float scale)
{
	std::vector<int> vec;

	float xmin = 9999999;
	float xmax = -9999999;
	float ymin = 9999999;
	float ymax = -9999999;
	//Find min maxes of posbuf 
	
	minMaxs(posBuf, xmin, xmax, ymin, ymax);

	float midImgX = .5 * width;
	float midImgY = .5 * height;
	float scaledX = scale * .5 * ((xmax + xmin)/2);
	float scaledY = scale * .5 * (ymax + ymin)/2;

	vec.push_back((midImgX - scaledX));
	vec.push_back((midImgY - scaledY));

	return  vec;
}
int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Usage: A1 meshfile" << endl;
		return 0;
	}
	string meshName(argv[1]);

	// Load geometry
	vector<float> posBuf; // list of vertex positions
	vector<float> norBuf; // list of vertex normals
	vector<float> texBuf; // list of vertex texture coords
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	cout << "Number of vertices: " << posBuf.size()/3 << endl;
	

/*
PSUEDOCODE

read args
make image pointer

find scale S and transform T

for vertices in PositionBuffer
	Apply Transform to fit
	Make scaled triangle from that

for Triangle in ScaledTripositionBuffer
	for pixel in boundingbox of triangle
		draw pixel
*/

//> ./A1 ../resources/bunny.obj output.png 512 512 1

	//Setup
	string filename = argv[2];
	int width = atof(argv[3]);
	int height = atof(argv[4]);
	int task = atoi(argv[5]);

	auto image = make_shared<Image>(width, height);




/////////////////////TASK ONE: BOUNDING BOXES////////////////////////////////


	//First, we must find the scale and transform values
	

	//Next, we must read and store the values
	
	//lets get a triangle at a time:

	if (task == 1) 
	{

		float S = findScale(posBuf, width, height);
		std::vector<int> T = findTrans(posBuf, width, height, S);
		int triCount = 0;
		for (int i = 0; i < posBuf.size(); i += 9)
		{
			//Make the 3 vertices for the Scaled Tri
			//T.at(0) = 239;
			//T.at(1) = 170;
			float xOffset = S * posBuf.at(0);
			float yOffset = S * posBuf.at(1);

			Point A(S * posBuf.at(i) + T.at(0), S * posBuf.at(i + 1) + T.at(1));
			Point B(S * posBuf.at(i + 3) + T.at(0), S * posBuf.at(i + 4) + T.at(1));
			Point C(S * posBuf.at(i + 6) + T.at(0), S * posBuf.at(i + 7) + T.at(1));

			//Make Triangle, Bounding Box
			Triangle tri(A, B, C);
			BoundingBox box(tri);

			//Now, we have all we need for drawing the bounding box for this specific Tri
			for (int y = box.ymin; y < box.ymax; ++y) {
				for (int x = box.xmin; x < box.xmax; ++x)
				{
					float r = RANDOM_COLORS[triCount % 7][0] * 255;
					float g = RANDOM_COLORS[triCount % 7][1] * 255;
					float b = RANDOM_COLORS[triCount % 7][2] * 255;

					image->setPixel(x, y, r, g, b);
				}
			}
			triCount += 1;
		}
	}

	//////////////////////////TASK 2: DRAWING TRIANGLES //////////////////////////////
	if (task == 2)
	{

		float S = findScale(posBuf, width, height);
		std::vector<int> T = findTrans(posBuf, width, height, S);
		int triCount = 0;
		for (int i = 0; i < posBuf.size(); i += 9)
		{
			//Make the 3 vertices for the Scaled Tri
			//T.at(0) = 239;
			//T.at(1) = 170;
			Point A(S * posBuf.at(i) + T.at(0), S * posBuf.at(i + 1) + T.at(1));
			Point B(S * posBuf.at(i + 3) + T.at(0), S * posBuf.at(i + 4) + T.at(1));
			Point C(S * posBuf.at(i + 6) + T.at(0), S * posBuf.at(i + 7) + T.at(1));

			//Make Triangle, Bounding Box
			Triangle tri(A, B, C);
			BoundingBox box(tri);

			//Now, weave to get the barycentric coefficients
	
			for (int y = box.ymin; y < box.ymax; ++y) {
				for (int x = box.xmin; x < box.xmax; ++x)
				{
					//Now, we have to get the barycentric coefficients
					Point p(x, y);
					float u;
					float v;
					float w;
					Barycentric(A, B, C, u, v, w, p);
					if ((u + v + w) <= 1.0005 && (u + v + w) > 0)
					{
						float r = RANDOM_COLORS[triCount % 7][0] * 255;
						float g = RANDOM_COLORS[triCount % 7][1] * 255;
						float b = RANDOM_COLORS[triCount % 7][2] * 255;
						image->setPixel(x, y, r, g, b);
					}
				}
			}
			triCount += 1;
		}
	}
	///////////////END TASK 2

	/////////////////////TASK 3: INTERPOLATING VERTEX COLORS///////////////////////////////

	if (task == 3)
	{
		scaleBuffer(posBuf, width, height);
		//translateToMid(posBuf, width, height);

		int triCount = 0;
		for (int i = 0; i < posBuf.size(); i += 9)
		{

			//MAKE NEW RBG VALUES FOR EACH VERTEX
			float r = RANDOM_COLORS[triCount % 7][0] * 255;
			float g = RANDOM_COLORS[triCount % 7][1] * 255;
			float b = RANDOM_COLORS[triCount % 7][2] * 255;
			Point A(posBuf.at(i),posBuf.at(i + 1),r,g,b);
			triCount += 1;

			r = RANDOM_COLORS[triCount % 7][0] * 255;
			g = RANDOM_COLORS[triCount % 7][1] * 255;
			b = RANDOM_COLORS[triCount % 7][2] * 255;
			Point B(posBuf.at(i + 3),posBuf.at(i + 4), r, g, b);
			triCount += 1;

			r = RANDOM_COLORS[triCount % 7][0] * 255;
			g = RANDOM_COLORS[triCount % 7][1] * 255;
			b = RANDOM_COLORS[triCount % 7][2] * 255;
			Point C(posBuf.at(i + 6),posBuf.at(i + 7), r, g, b);
			triCount += 1;

			//Make Triangle, Bounding Box
			Triangle tri(A, B, C);
			BoundingBox box(tri);

			//Now, weave to get the barycentric coefficients

			for (int y = box.ymin; y < box.ymax; ++y) {
				for (int x = box.xmin; x < box.xmax; ++x)
				{
					//Now, we have to get the barycentric coefficients
					Point p(x, y);
					float u;
					float v;
					float w;
					Barycentric(A, B, C, u, v, w, p);
					if ((u + v + w) <= 1.0005 && (u + v + w) > 0)
					{
						r = tri.v1.r * u + tri.v2.r * v + tri.v3.r * w;
						g = tri.v1.g * u + tri.v2.g * v + tri.v3.g * w;
						b = tri.v1.b * u + tri.v2.b * v + tri.v3.b * w;
						image->setPixel(x, y, r, g, b);
					}
				}
			}
		}
	}

	/////////////////////END TASK 3 ///////////////////////////////


	////////////////////////Task 4: Verticle color////////////////////////

	if (task == 4)
	{

		float S = findScale(posBuf, width, height);
		std::vector<int> T = findTrans(posBuf, width, height, S);
		int triCount = 0;
		//Set R Value and B Value for whole object now.

		float r = 0; // Increment R;
		float g = 0;
		float b = 255;
		for (int i = 0; i < posBuf.size(); i += 9)
		{
			//Make the 3 vertices for the Scaled Tri
			//T.at(0) = 239;
			//T.at(1) = 170;

			//MAKE NEW RBG VALUES FOR EACH VERTEX

			Point A(S * posBuf.at(i) + T.at(0), S * posBuf.at(i + 1) + T.at(1), r, g, b);
			triCount += 1;

			Point B(S * posBuf.at(i + 3) + T.at(0), S * posBuf.at(i + 4) + T.at(1), r, g, b);
			triCount += 1;

			Point C(S * posBuf.at(i + 6) + T.at(0), S * posBuf.at(i + 7) + T.at(1), r, g, b);
			triCount += 1;

			//Make Triangle, Bounding Box
			Triangle tri(A, B, C);
			BoundingBox box(tri);


			//FIND XMIN AND MAXS FOR OBJ, SO WE KNOW HOW MUCH TO CHANGE THE COLOR EACH TIME TO HAVE SMOOTH CHANGE
			//////////////
			float xmin = 9999999;
			float xmax = -9999999;
			float ymin = 9999999;
			float ymax = -9999999;
			//Find min maxes of posbuf
			int x = 1;
			for (int i = 0; i < posBuf.size(); i++)
			{
				if (x % 3 == 0) {}
				else if (x % 2 == 0)
				{
					//cout << "VERT " << posBuf.at(i) << "\n";
					if (xmin > posBuf.at(i)) { xmin = posBuf.at(i); }
					if (xmax < posBuf.at(i)) { xmax = posBuf.at(i); }
				}
				else
				{
					if (ymin > posBuf.at(i)) { ymin = posBuf.at(i); }
					if (ymax < posBuf.at(i)) { ymax = posBuf.at(i); }
				}
				x += 1;
			}

			float scaleX = width / (xmax - xmin);
			float scaleY = height / (ymax - ymin);
			float scale = min(scaleX, scaleY);

			float objHeight = scale * (ymax - ymin);
			float increment = 255/objHeight;

			//////////////
			float r = 0;
			float g = 0;
			float b = 255;
			float h = scale * (ymax - ymin);
			for (int y = int(box.ymin); y < int(box.ymax); ++y) {
				float percentY = ((y - ymin) / h);
				r = percentY * (255);
				b = 255 - percentY * (255);

				if (r > 255 || b > 255) { cout << "WARNING WARNING\n"; }
				for (int x = int(box.xmin); x < int(box.xmax); ++x)
				{
					//Now, we have to get the barycentric coefficients
					Point p(x, y);
					float u;
					float v;
					float w;
					Barycentric(A, B, C, u, v, w, p);

					if ((u + v + w) <= 1.0005 && (u + v + w) > 0)
					{
						image->setPixel(x, y, r, g, b);
					}
				}
			}
		}
	}





	image->writeToFile(filename);

	return 0;
}
