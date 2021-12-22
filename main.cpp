

#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <math.h>
//#define NOMINMAX
#define wx 1000
#define wy 1000

using namespace sf;
using namespace std;

void draw_line(int x1, int y1, int x2, int y2, RenderWindow& window) {  // ��������� �����
	Vertex line[] = {
		Vertex(Vector2f(x1, y1)),
		Vertex(Vector2f(x2, y2))
	};
	window.draw(line, 2, Lines);
}

void put_pixel(int x, int y, Color color, RenderWindow& window) { //������� ��������� �����
	RectangleShape rectangle1(Vector2f(3.f, 3.f)); //��������������� �������� 3�3
	rectangle1.move(x, y); //������� � ������� � ������������ x, y
	rectangle1.setFillColor(color); //����� ����
	window.draw(rectangle1); //���������
}

void myline(int x1, int y1, int x2, int y2, Color co, RenderWindow& window) //������� ��������� �����
{

	const int N = abs(x2 - x1); //���������� �� X
	const int M = abs(y2 - y1); //���������� �� Y
	//���������� ����������� ����� ��� �������� ������� ������� ����� �����
	const int signX = x1 < x2 ? 1 : -1; //���� �����-�������, �� 1, ����� -1
	const int signY = y1 < y2 ? 1 : -1; //���� ������-����, �� 1, ����� -1
	int buf = N - M; //������� ���������� �� X � Y
	put_pixel(x2, y2, co, window); //����� ������� ��������� ����� ����� �����
	while (x1 != x2 || y1 != y2) //���� �� ����� �� ������ �� ����� �����
	{
		put_pixel(x1, y1, co, window); //����� ������� ��������� ����� �����
		int buf_ = buf * 2; //������������� �������� ��������� ������� ���������� X � Y
		if (buf_ > -M) //���� ������ ���������� �� Y � ������������� ���������
		{
			buf -= M; //�������� ���������� �� Y �� ������� ���������� �� X � Y 
			x1 += signX; //������� ������� ������� ����� �� X
		}
		if (buf_ < N) //���� ������ ���������� �� X 
		{
			buf += N; //���������� ���������� �� X � ������� ���������� �� X � Y 
			y1 += signY; //������� ������� ������� ����� �� Y
		}
	}
}

// ����������
class point
{
	public:
		double x; double y; double z;
};

// �������
class vertex
{
	public:
		point worldcoord; // ������� ����������
		point viewcoord; // ������� ����������
		// �������� ����������
		double X; double Y; 
		vertex() {}

		vertex(double x, double y, double z) // ����� ������� ����������
		{
			worldcoord.x = x; worldcoord.y = y; worldcoord.z = z;
		}

		void setviewcoord(point viewpoint, double vert, double hor, double d) // ����������� ������� ���������� � �������
		{
			X = hor;
			Y = vert;
			//�������������� ��������� ����� ���������� � ������� - ��������� ����� ��, ���� � ��
			double temp = atan(1.0) / 45.0;
			double ro = viewpoint.x * temp;
			double teta = viewpoint.y * temp;
			double fi = viewpoint.z * temp;

			// ������� 
			double wmatrix[4] = { worldcoord.x, worldcoord.y, worldcoord.z, 1 }; // ������� ������� ���������
			double vmatrix[4][4] = {	{-sin(teta),	-cos(fi)*cos(teta),		-sin(fi)*cos(teta),		0},
										{cos(teta),		-cos(fi)*sin(teta),		-sin(fi)*sin(teta),		0},
										{0,				sin(fi),				-cos(fi),				0},
										{0,				0,						ro,						1} 
									}; // �������������� ������� ���������
			// ������� �������� ��������������
			double ematrix[4];

			// ���������� 
			for (int i = 0; i < 4; i++)
			{
				ematrix[i] = 0;
				for (int j = 0; j < 4; j++)
				{
					ematrix[i] += wmatrix[j] * vmatrix[j][i];
				}
			}

			// ����� ������� ���������� �� ���������� ������������ ������
			viewcoord.x = ematrix[0];
			viewcoord.y = ematrix[1];
			viewcoord.z = ematrix[2];

			//���������� �������� ���������
			X += d*viewcoord.x/viewcoord.z;
			Y += d*viewcoord.y/viewcoord.z;
		}
};

// и���
class edge
{
	public:
		// ������ � ����� �����
		vertex start; 
		vertex end;
		edge() {}
		edge(vertex svertex, vertex evertex)
		{
			start = svertex; end = evertex;
		}

		//��������� �����
		void drawedge(Color color, RenderWindow& window)
		{
			myline(start.X, start.Y, end.X, end.Y, color, window);
		}
};

// ������
class surface
{
	private:
		vector<vertex> vlist; // ������ ������
		vector<vector<int>> elist; // ������ ����
	public:
		point viewpoint;

		void load(const char* filename)
		{
			ifstream f(filename);
			if (!f.is_open()) // ���� ���� �� ��� ������
			{
				cout << "���� �� ����� ���� ������ ��� ������\n"; // ���������� ��������������� ���������
				exit(1); // ��������� ����� �� ���������
			}

			// ��������� ���������� ������
			int vcount; // ���������� ������
			int coord[3]; // 3 ���������� x, y, z
			f >> vcount;
			for (int i = 0; i < vcount; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					f >> coord[j];
				}
				vlist.push_back(vertex(coord[0], coord[1], coord[2])); // ��������� � ������ ������
			}

			//�������� ���������� � ������ � ��������, ����� �������� ��� ���������
			//string ecount; // ���������� ���� 
			int ecount; // ���������� ����
			int vert[2]; // 2 �������
			f >> ecount;
			for (int i = 0; i < ecount; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					f >> vert[j];
				}
				// ��������� � ������ ����
				elist.push_back({});
				elist[i].push_back(vert[0]);
				elist[i].push_back(vert[1]);
			}
			f.close(); // ��������� ����
		}

		//��������� ����� ����������
		void setviewpoint(double ro, double theta, double fi)
		{
			viewpoint.x = ro; viewpoint.y = theta; viewpoint.z = fi;
		}

		//��������� ��������� ������
		void drawsurface(double vert, double hor, double d, Color color, RenderWindow& window)
		{
			// �������������� ��������� ������ �� ������ ������
			for (int i = 0; i < vlist.size(); i++)
			{
				vlist[i].setviewcoord(viewpoint, vert, hor, d);
			}

			// ��������� ����
			for (int i = 0; i < elist.size(); i++)
			{
				edge edge1 = edge(vlist[elist[i][0] - 1], vlist[elist[i][1] - 1]);
				edge1.drawedge(color, window);
			}
		}
};

int main()
{
	RenderWindow window(VideoMode(wx, wy), "SFML Works!!!"); //������� ���� ����������

	while (window.isOpen()) // ������� ���� ����������: �����������, ���� ������� ����
	{
		Event event; // ��������� ������� � �����
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) // ������� �������� ����
				window.close();
		}
		window.clear(Color(5, 0, 50, 0)); // ���� ����

		// ���������� ���������� 
		Color color = Color(105, 0, 200); // ���� �����
		double P1 = 0.3; // ���� ���� 
		double P2 = 80.0; // ���� ��
		const char* inputfile = "C:\\Users\\rybalko_pu\\Documents\\Study\\3 ����\\2 ������\\������������ �������\\��� 3\\Cube.dat"; // ���� �� ����� ���������
		//const char* inputfile = "C:\\Users\\rybalko_pu\\Documents\\Study\\3 ����\\2 ������\\������������ �������\\��� 3\\House.dat";
		surface s1;
		s1.load(inputfile);
		for (int i = 1; i < 10; i++) { // �������� ����� ����������
			s1.setviewpoint(1000, P1, P2); // ����� ����������
			s1.drawsurface(wx/2, wy/2, 3000, color, window); // ��������� ������
			// �������� ����� ����������
			P1 += 0.5;
			P2 += 5;
			Sleep(500); // �������� �� �������
			window.display(); // ��������� ����
			window.clear(Color(5, 0, 50, 0)); // ���� ����
		}
	}
	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
