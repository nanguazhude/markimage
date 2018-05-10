#ifndef __PDF_TO_IMAGE__HPP
#define __PDF_TO_IMAGE__HPP
 
#include "pdf_to_image.private.h"
 
#ifdef __cplusplus
#include <memory>
std::shared_ptr< FirstPageImage > makeMUPDFFirstPage() {
	return std::shared_ptr< FirstPageImage >(new FirstPageImage{},
		[](FirstPageImage * arg) {
		if (arg->freepack) {
			arg->freepack(arg);
		}
		delete arg;});
}
#endif

#endif


