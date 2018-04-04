#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "NiftiImage.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class niftiExampleApp : public App {
  public:
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

void niftiExampleApp::setup()
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

void niftiExampleApp::mouseDown( MouseEvent event )
{
}

void niftiExampleApp::createImage() {
	std::printf("Slicing image along z = %d", mSliceZ);
	uint8_t* data = mMRI->readSubvolume<uint8_t>(mSliceZ, 0, 0, 0, mSliceZ, mMRI->ny(), mMRI->nz(), 0);
	std::vector<uint8_t> vec(*data, 320*320);
	/*
	for (int i = 0; i < mMRI->nz() * mMRI->ny(); i++) {
	std::printf("Intensity: %d", data[i]);
	}
	*/
	mSurface = ci::Surface8u(mMRI->nz(), mMRI->ny(), false, cinder::SurfaceChannelOrder(cinder::SurfaceChannelOrder::RGBA));
	for (int j = 0; j < mMRI->ny(); j++) {
		for (int i = 0; i < mMRI->nz(); i++) {
			char c = data[i + j * mMRI->nz()];
			mSurface.setPixel(glm::ivec2(i, j), ci::ColorT<uint8_t>(c, c, c));
		}
	}
}

void niftiExampleApp::update()
{
}

void niftiExampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::TextureRef texture = gl::Texture::create(mSurface);
	gl::draw(texture);
}

void niftiExampleApp::keyDown(KeyEvent event) {
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

CINDER_APP( niftiExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {settings->setConsoleWindowEnabled(); })
