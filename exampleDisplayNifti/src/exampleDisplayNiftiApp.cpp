#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "NiftiImage.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class exampleDisplayNiftiApp : public App {
  void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void keyDown(KeyEvent event) override;
	void createImage();

	std::shared_ptr<NiftiImage> mMRI;
	ci::Surface8u mSurface;
	int mSliceZ;
};

void exampleDisplayNiftiApp::setup()
{
	mSliceZ = 0;
	mMRI = std::make_shared<NiftiImage>();
	mMRI->open(app::getAssetPath("sub-001_T1w.nii").string().c_str(), NiftiImage::NIFTIIMAGE_MODES::NIFTI_READ);

	std::printf("Total number of dimensions %d\n", mMRI->ndim());
	std::printf("Voxels are %d x %d x %d x %d\n", mMRI->nx(), mMRI->ny(), mMRI->nz(), mMRI->nt());
	std::printf("Voxels per slice %d\n", mMRI->voxelsPerSlice());
	std::printf("Voxels per volume %d\n", mMRI->voxelsPerVolume());
	std::printf("Total Voxels: %d\n", mMRI->voxelsTotal());

	createImage();
}

void exampleDisplayNiftiApp::mouseDown( MouseEvent event )
{
}

void exampleDisplayNiftiApp::keyDown(KeyEvent event) {
	if (event.getCode() == KeyEvent::KEY_UP) {
		mSliceZ++;
	}
	else if (event.getCode() == KeyEvent::KEY_DOWN) {
		mSliceZ--;
	}
	if (mSliceZ < 0) {
		mSliceZ = 0;
	}
	if (mSliceZ > 255) {
		mSliceZ = 255;
	}
	createImage();
}

void exampleDisplayNiftiApp::createImage() {
	std::printf("Slicing image along z = %d", mSliceZ);
	//uint8_t* data = mMRI->readSubvolume<uint8_t>(mSliceZ, 0, 0, 0, mSliceZ, mMRI->ny(), mMRI->nz(), 0);
	uint8_t* data = mMRI->readVolume<uint8_t>(128);
	size_t dataLength = 320 * 320;
	std::vector<uint8_t> vec(data, data + dataLength);
	mSurface = ci::Surface8u(mMRI->nz(), mMRI->ny(), false, cinder::SurfaceChannelOrder(cinder::SurfaceChannelOrder::RGBA));
	for (int j = 0; j < mMRI->ny(); j++) {
		for (int i = 0; i < mMRI->nz(); i++) {
			char c = data[i + j * mMRI->nz()];
			mSurface.setPixel(glm::ivec2(i, j), ci::ColorT<uint8_t>(c, c, c));
		}
	}
	for (int j = 0; j < mMRI->ny(); j++) {
		for (int i = 0; i < mMRI->nz(); i++) {
			int index = i + j * mMRI->nz();
			std::printf("NIFTI Intensity: %d \t Surface Value: %d\n", data[index], mSurface.getPixel(glm::ivec2(i,j)).r);
		}
	}
}

void exampleDisplayNiftiApp::update()
{
}

void exampleDisplayNiftiApp::draw()
{
  gl::clear( Color( 0, 0, 0 ) );
  gl::TextureRef texture = gl::Texture::create(mSurface);
  gl::draw(texture);
}

CINDER_APP(exampleDisplayNiftiApp, RendererGl, [=](cinder::app::App::Settings* settings) { settings->setConsoleWindowEnabled(); })
