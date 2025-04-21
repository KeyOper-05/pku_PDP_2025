#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <libgen.h>  // dirname()
#define BMP_HEADER_USELESS 10
using namespace std;
struct pixel_24bpp {
	unsigned char channels[3];
};
struct dib_header {
	int header_size;
	signed int width; //in pixels
	signed int height;
	unsigned short color_plane;
	unsigned short bits_p_pixel;
	char compression_method[4];
	int image_size;
	signed int horizontal_res;
	signed int vertical_res;
	int color_palette;
	int imp_colors;
};

class bmp_picture {
	string filename;
	dib_header header;
	int initial_padding;
	char useless_header[BMP_HEADER_USELESS];
	int rowsize;
	int bytes_p_pixel;
	int pixelArraySize;
	int pixelArrayOffset;
	vector<vector<pixel_24bpp>> pixelArray;
	fstream pic;
	string outdst;
public:
	bmp_picture(const string& filename, const string& outdst) {
		this->filename = filename;
		this->outdst = outdst;
		pic.open(filename, ios::in | ios::out | ios::binary);
		pic.read(useless_header, BMP_HEADER_USELESS*sizeof(char)); //skip "BM" and other unnecessary headers
		pic.read((char *)&pixelArrayOffset, sizeof(int));
		pic.read((char*)&header,sizeof(dib_header));
		rowsize = ceil(((float)header.bits_p_pixel * (float)header.width)/32) * 4;
		pixelArraySize = rowsize * abs(header.height);
		bytes_p_pixel = header.bits_p_pixel / 8;
		initial_padding = rowsize - header.width * bytes_p_pixel; //
	}
	vector<vector<pixel_24bpp>> rotated_pixelArray;
	dib_header rotatedHeader;
	void get_pixel_array() {
		pixel_24bpp temp;
		vector<pixel_24bpp> rowTmp;
		pic.seekg(pixelArrayOffset,ios::beg);
		for(int i=1;i<=header.height;i++) {
			rowTmp.clear();
			for(int j=1;j<=header.width;j++) {
				pic.read((char* )&temp,sizeof(pixel_24bpp));
				rowTmp.push_back(temp);
			}
			pixelArray.push_back(rowTmp);
			pic.seekg(initial_padding,ios::cur);
		}
	}
	void generate_rotation() {
		if(pixelArray.empty()) return;
		vector<pixel_24bpp> Tmp;
		for(int x=0;x<header.width;x++) {
			Tmp.clear();
			for(int y=0;y<header.height;y++) {
				Tmp.push_back(pixelArray[y][header.width-x-1]);
			}
			rotated_pixelArray.push_back(Tmp);
		}
		rotatedHeader = header;
		rotatedHeader.width = header.height;
		rotatedHeader.height = header.width;
		rotatedHeader.horizontal_res = header.vertical_res;
		rotatedHeader.vertical_res = header.horizontal_res;
	}
	void rotate() {
		get_pixel_array();
		generate_rotation();
		fstream wfile;
		wfile.open(outdst, ios::out | ios::binary );
		wfile.write(useless_header,sizeof(useless_header));
		wfile.write((char *)&pixelArrayOffset,sizeof(int));
		wfile.write((char *)&rotatedHeader,sizeof(rotatedHeader));
		wfile.seekp(pixelArrayOffset,ios::beg);
		char padding = 0;
		int rotated_padding = ceil(((float)header.bits_p_pixel * (float)rotatedHeader.width)/32) * 4 - rotatedHeader.width * bytes_p_pixel;
		//cout<<rotated_padding;
		for(int i=0;i<rotatedHeader.height;i++) {
			for(int j=0;j<rotatedHeader.width;j++) {
				wfile.write((char*)&rotated_pixelArray[i][j],sizeof(pixel_24bpp));
			}
			wfile.write(&padding,rotated_padding);
		}
		wfile.close();
	}
	~bmp_picture() {
		pic.close();
	}
};
int main(int argc, char*argv[]) {
	char path[1024];
	realpath(argv[0], path);  // 得到可执行文件的绝对路径
	string dir = dirname(path);
	string fulldst = dir + "/src.bmp";
	string outdst = dir + "/dest.bmp";
	bmp_picture pic(fulldst,outdst);
	pic.rotate();
}