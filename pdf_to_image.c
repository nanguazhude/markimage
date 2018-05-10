#include <mupdf/fitz.h>
#include <mupdf/pdf.h>
#include "pdf_to_image.hpp"

static void free_pack(void * data) {

	fz_context * ctx = (fz_context *)(((FirstPageImage*)(data))->pack_0);
	fz_pixmap * pix = (fz_pixmap *)(((FirstPageImage*)(data))->pack_1);
	fz_document *doc = (fz_document *)(((FirstPageImage*)(data))->pack_2);

	fz_drop_pixmap(ctx, pix);
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
}

extern void get_pdf_first_page(const char * varFileName, FirstPageImage * varAns) {
	fz_context *ctx = NULL ;
	fz_document *doc = NULL ;
	fz_pixmap *pix = NULL ;
	fz_matrix ctm = fz_identity;
	int page_count = 0;
	int page_number = 0;

	if (varAns) {
		varAns->data = NULL;
		varAns->freepack = NULL;
		varAns->height = 0;
		varAns->width = 0;
		varAns->stride = 0;
		varAns->pack_0 = NULL;
		varAns->pack_1 = NULL;
		varAns->pack_2 = NULL;
	}
	else {
		return;
	}

	//创建上下文
	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx) {
		return;
	}

	//注册文档控制
	fz_try(ctx)
		fz_register_document_handlers(ctx);
	fz_catch(ctx)
	{
		fz_drop_context(ctx);
		return;
	}

	//打开文档
	fz_try(ctx)
		doc = fz_open_document(ctx, varFileName);
	fz_catch(ctx)
	{
		fz_drop_context(ctx);
		return;
	}

	//取得总的页数
	fz_try(ctx)
		page_count = fz_count_pages(ctx, doc);
	fz_catch(ctx)
	{
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return;
	}

	if ((page_number < 0) || (page_number >= page_count)) {
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return;
	}

	fz_try(ctx)
		pix = fz_new_pixmap_from_page_number(ctx,
			doc,
			page_number,
			&ctm,
			fz_device_rgb(ctx),
			0);
	fz_catch(ctx) {
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return;
	}

	varAns->data = fz_pixmap_samples(ctx, pix);
	varAns->width = fz_pixmap_width(ctx, pix);
	varAns->height = fz_pixmap_height(ctx, pix);
	varAns->stride = fz_pixmap_stride(ctx, pix);

	varAns->pack_0 = ctx;
	varAns->pack_1 = pix;
	varAns->pack_2 = doc;
	varAns->freepack = &free_pack;
}


