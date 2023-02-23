/*******************************************************************************
 * ImageLoader.cpp :   texture / image loader
 *******************************************************************************
 *
 *  Author(s):      Henry P, Michael R
 *  Project:		Comp 308
 *  Date:			2014
 *
 ******************************************************************************/
 
/******************************************************************************/
/* Header files                                                               */
/******************************************************************************/

#include "ImageLoader.h"
#include "string.h"

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/*  Open texture file			                                              */
/******************************************************************************/
GLuint openTexture(const char* filename) {
	
	GLuint texName;
	TextureInfo t;
	unsigned int i;
	
	for (i = 0; i < strlen(filename); i++) {
		if (filename[i] == '.') {
			break;
		}
	}
	
	char* ncs = new char[i+5];
	strcpy(ncs, filename);
	char extension[5];
	strcpy(extension, &filename[i + 1]);
	
	if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0)
		loadTextureFromJPEG(ncs, &t);
	else if (strcmp(extension, "png") == 0)
		loadTextureFromPNG(ncs, &t);
	else {
		printf("Invalid format. Only supports JPEG and PNG.\n");
		exit(1);
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	if (t.hasAlpha) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, t.textureData);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t.width, t.height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, t.textureData);
	}
	
	free(t.textureData);
	delete(ncs);
	return texName;
	
} /* openTexture */

/******************************************************************************/
/*  Open texture info file		                                              */
/******************************************************************************/
TextureInfo openTextureInfo(const char* filename) {
	TextureInfo t;
	unsigned int i;
	for (i = 0; i < strlen(filename); i++) {
		if (filename[i] == '.') {
			break;
		}
	}
	char* ncs = new char[i+5];
	strcpy(ncs, filename);
	char extension[5];
	strcpy(extension, &filename[i + 1]);
	if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0)
		loadTextureFromJPEG(ncs, &t);
	else if (strcmp(extension, "png") == 0)
		loadTextureFromPNG(ncs, &t);
	else {
		printf("Invalid format. Only supports JPEG and PNG.\n");
		exit(1);
	}
	delete(ncs);
	return t;
	
} /* openTextureInfo */


/******************************************************************************/
/*  Open cube map for sky box	                                              */
/******************************************************************************/
GLuint openCubeMap(const char* xp, const char* xn, const char* yp, const char* yn, const char* zp, const char* zn) {
	
	GLuint texName;
	TextureInfo xpt = openTextureInfo(xp);
	TextureInfo xnt = openTextureInfo(xn);
	TextureInfo ypt = openTextureInfo(yp);
	TextureInfo ynt = openTextureInfo(yn);
	TextureInfo zpt = openTextureInfo(zp);
	TextureInfo znt = openTextureInfo(zn);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texName);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texName);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLuint rgbType = xpt.hasAlpha ? GL_RGBA : GL_RGB;

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, rgbType, xpt.width, xpt.height, 0, rgbType,	GL_UNSIGNED_BYTE, xpt.textureData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, rgbType, xnt.width, xnt.height, 0, rgbType,	GL_UNSIGNED_BYTE, xnt.textureData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, rgbType, ypt.width, ypt.height, 0, rgbType,	GL_UNSIGNED_BYTE, ypt.textureData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, rgbType, ynt.width, ynt.height, 0, rgbType,	GL_UNSIGNED_BYTE, ynt.textureData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, rgbType, zpt.width, zpt.height, 0, rgbType,	GL_UNSIGNED_BYTE, zpt.textureData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, rgbType, znt.width, znt.height, 0, rgbType,	GL_UNSIGNED_BYTE, znt.textureData);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

	free(xpt.textureData);
	free(xnt.textureData);
	free(ypt.textureData);
	free(ynt.textureData);
	free(zpt.textureData);
	free(znt.textureData);

	return texName;
	
} /* openCubeMap */


/******************************************************************************/
/*  Load jpeg texture utility	                                              */
/******************************************************************************/
int loadTextureFromJPEG(char* filename, TextureInfo *info) {
	FILE *fd;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char * line;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	if (0 == (fd = fopen(filename, "rb"))) {
		printf("Error opening file");
		exit(1);
	}
	info->filename = filename;
	jpeg_stdio_src(&cinfo, fd);
	jpeg_read_header(&cinfo, TRUE);
	unsigned int size = cinfo.image_width;
	info->height = cinfo.image_height;
	info->width = cinfo.image_width;
	info->format = GL_RGB;
	info->hasAlpha = false;
	unsigned char* image = (unsigned char*) malloc(sizeof(char) * 3 * cinfo.image_height * cinfo.image_width);
	jpeg_start_decompress(&cinfo);
	while (cinfo.output_scanline < cinfo.output_height) {
		line = image
				+ (3 * size) * cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, &line, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	info->textureData = (unsigned char*) malloc(sizeof(char) * 3 * cinfo.image_height * cinfo.image_width);
	for(int y = info->height-1; y >= 0; y--){
		for(int x = 0; x < info->width*3; x++){
				info->textureData[(info->height-y-1)*(info->width*3)+x] = image[y*info->width*3+x];
		}
	}
	free(image);

	return 0;
	
} /* loadTextureFromJPEG */


/******************************************************************************/
/*  Load png texture utility	                                              */
/******************************************************************************/
int loadTextureFromPNG(char* filename , TextureInfo* t){
	png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL){
        printf("Error opening %s\n",filename);
        exit(1);
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
    	printf("LibPNG did something terrible!\n");
        fclose(fp);
        exit(1);
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
    	printf("LibPNG did something terrible!\n");
        fclose(fp);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        exit(1);
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
    	printf("LibPNG did something terrible!\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        exit(1);
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);
    t->width = png_get_image_width(png_ptr,info_ptr);
    t->height = png_get_image_height(png_ptr,info_ptr);
    switch (png_get_color_type(png_ptr,info_ptr)) {
        case PNG_COLOR_TYPE_RGBA:
            t->hasAlpha = true;
            break;
        case PNG_COLOR_TYPE_RGB:
        	t->hasAlpha = false;
            break;
        default:
            printf("Invalid PNG.\n");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            exit(1);
    }
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    t->textureData = (unsigned char*) malloc(row_bytes * t->height);
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    for (int i = 0; i < t->height; i++) {
        memcpy(t->textureData+(row_bytes * (t->height-1-i)), row_pointers[i], row_bytes);
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(fp);
    return 0;
	
} /* loadTextureFromPNG */



