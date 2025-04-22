#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <libgen.h>  // dirname()
#define BMP_HEADER_USELESS 10
#define BMP_HEADER_TAIL_RESERVE 200
using namespace std;
struct pixel_24bpp {
	unsigned char channels[3];
};
struct pixel_32bpp {
	unsigned char channels[4];
};
#pragma pack(1)
struct bmp_header {
	unsigned char useless_header[BMP_HEADER_USELESS];
	int pixelArrayOffset;
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
	unsigned char useless_tail[BMP_HEADER_TAIL_RESERVE];
};
bmp_header headerReader(fstream& file) {
	bmp_header header;
	//分片读入
	file.read((char *) header.useless_header, sizeof(header.useless_header));
	file.read((char *) &header.pixelArrayOffset, sizeof(header.pixelArrayOffset));
	//bmp_header finished
	//dib_header
	file.read((char *) &header.header_size, sizeof(header.header_size));
	file.read((char *) &header.width, sizeof(header.width));
	file.read((char *) &header.height, sizeof(header.height));
	file.read((char *) &header.color_plane, sizeof(header.color_plane));
	file.read((char *) &header.bits_p_pixel, sizeof(header.bits_p_pixel));
	file.read((char *) &header.compression_method, sizeof(header.compression_method));
	file.read((char *) &header.image_size, sizeof(header.image_size));
	file.read((char *) &header.horizontal_res, sizeof(header.horizontal_res));
	file.read((char *) &header.vertical_res, sizeof(header.vertical_res));
	file.read((char *) &header.color_palette, sizeof(header.color_palette));
	file.read((char *) &header.imp_colors, sizeof(header.imp_colors));
	file.read((char *) header.useless_tail, header.header_size - 40);
	return header;
}
template<typename imageType>
class bmp_picture {
	bmp_header header;
	bmp_header rotatedHeader;
	int initial_padding;
	int rowsize;
	int bytes_p_pixel;
	int pixelArraySize;
	vector<vector<imageType>> pixelArray;
	vector<vector<imageType>> rotated_pixelArray;
	fstream& pic;
	string outdst;
public:
	bmp_picture(fstream& file, const string& outdst, bmp_header header): pic(file), outdst(outdst), header(header) {
		rowsize = ceil(((float)header.bits_p_pixel * (float)header.width)/32) * 4;
		pixelArraySize = rowsize * abs(header.height);
		bytes_p_pixel = header.bits_p_pixel / 8;
		initial_padding = rowsize - header.width * bytes_p_pixel; //
	}

	void get_pixel_array() {
		imageType temp;
		vector<imageType> rowTmp;
		pic.seekg(header.pixelArrayOffset,ios::beg);
		for(int i=1;i<=header.height;i++) {
			rowTmp.clear();
			for(int j=1;j<=header.width;j++) {
				pic.read((char* )&temp,sizeof(imageType));
				rowTmp.push_back(temp);
			}
			pixelArray.push_back(rowTmp);
			pic.seekg(initial_padding,ios::cur);
		}
	}
	void generate_rotation() {
		if(pixelArray.empty()) return;
		vector<imageType> Tmp;
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
		wfile.write((char *)&rotatedHeader,header.pixelArrayOffset);
		wfile.seekp(header.pixelArrayOffset,ios::beg);
		char padding = 0;
		int rotated_padding = ceil(((float)header.bits_p_pixel * (float)rotatedHeader.width)/32) * 4 - rotatedHeader.width * bytes_p_pixel;
		//cout<<rotated_padding;
		for(int i=0;i<rotatedHeader.height;i++) {
			for(int j=0;j<rotatedHeader.width;j++) {
				wfile.write((char*)&rotated_pixelArray[i][j],sizeof(imageType));
			}
			wfile.write(&padding,rotated_padding);
		}
		wfile.close();
	}
	~bmp_picture() {}
};
int main(int argc, char*argv[]) {
	char path[1024];
	realpath(argv[0], path);  // 得到可执行文件的绝对路径
	string dir = dirname(path);
	string fulldst = dir + "/src.bmp";
	string outdst = dir + "/dest.bmp";
	fstream picF;
	picF.open(fulldst, ios::in | ios::binary);
	bmp_header header = headerReader(picF);
	cout << "header_size = " << header.header_size << endl;
	cout << "bits_p_pixel = " << header.bits_p_pixel << endl;
	cout << "pixelArrayOffset = " << header.pixelArrayOffset << endl;
	if(header.bits_p_pixel == 24){
		bmp_picture<pixel_24bpp> pic(picF,outdst,header);
		pic.rotate();
	} else if(header.bits_p_pixel == 32){
		bmp_picture<pixel_32bpp> pic(picF,outdst,header);
		pic.rotate();
	}
	picF.close();
}