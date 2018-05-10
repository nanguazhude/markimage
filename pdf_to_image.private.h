#ifdef __cplusplus
extern "C" {
#endif

typedef struct FirstPageImage {
	unsigned char * data;
	int width;
	int height;
	int stride;
	void * pack_0;
	void * pack_1;
	void * pack_2;
	void(*freepack)(void *);
#ifdef __cplusplus
	FirstPageImage() :
		width(0),
		height(0),
		stride(0),
		pack_0(nullptr),
		pack_1(nullptr),
		pack_2(nullptr),
		freepack(nullptr){}
#endif

}FirstPageImage;

void get_pdf_first_page(const char * varFileName, FirstPageImage * varAns);

#ifdef __cplusplus
}
#endif

