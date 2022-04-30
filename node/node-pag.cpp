#include <pag/file.h>
#include <pag/pag.h>
#include <assert.h>
#include <napi.h>

int64_t GetTimer() {
  static auto START_TIME = std::chrono::high_resolution_clock::now();
  auto now = std::chrono::high_resolution_clock::now();
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - START_TIME);
  return static_cast<int64_t>(ns.count() * 1e-3);
}

std::shared_ptr<pag::PAGFile> ReplaceImageOrText() {
  auto pagFile = pag::PAGFile::Load("/data/github.com/libpag/assets/test2.pag");
  if (pagFile == nullptr) {
    return nullptr;
  }
  for (int i = 0; i < pagFile->numImages(); i++) {
    auto pagImage = pag::PAGImage::FromPath("/data/github.com/libpag/assets/scene.png");
    pagFile->replaceImage(i, pagImage);
  }

  for (int i = 0; i < pagFile->numTexts(); i++) {
    auto textDocumentHandle = pagFile->getTextData(i);
    textDocumentHandle->text = "hah哈 哈哈哈哈👌";
    // Use special font
    auto pagFont = pag::PAGFont::RegisterFont("/data/github.com/libpag/resources/font/NotoSansSC-Regular.otf", 0);
    textDocumentHandle->fontFamily = pagFont.fontFamily;
    textDocumentHandle->fontStyle = pagFont.fontStyle;
    pagFile->replaceText(i, textDocumentHandle);
  }

  return pagFile;
}

int64_t TimeToFrame(int64_t time, float frameRate) {
  return static_cast<int64_t>(floor(time * frameRate / 1000000ll));
}

void BmpWrite(unsigned char* image, int imageWidth, int imageHeight, const char* filename) {
  unsigned char header[54] = {0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0,
                              0,    0,    0, 0, 0, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0,  0, 0, 0,
                              0,    0,    0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0};

  int64_t file_size = static_cast<int64_t>(imageWidth) * static_cast<int64_t>(imageHeight) * 4 + 54;
  header[2] = static_cast<unsigned char>(file_size & 0x000000ff);
  header[3] = (file_size >> 8) & 0x000000ff;
  header[4] = (file_size >> 16) & 0x000000ff;
  header[5] = (file_size >> 24) & 0x000000ff;

  int64_t width = imageWidth;
  header[18] = width & 0x000000ff;
  header[19] = (width >> 8) & 0x000000ff;
  header[20] = (width >> 16) & 0x000000ff;
  header[21] = (width >> 24) & 0x000000ff;

  int64_t height = -imageHeight;
  header[22] = height & 0x000000ff;
  header[23] = (height >> 8) & 0x000000ff;
  header[24] = (height >> 16) & 0x000000ff;
  header[25] = (height >> 24) & 0x000000ff;

  char fname_bmp[128];
  sprintf(fname_bmp, "%s.bmp", filename);

  FILE* fp;
  if (!(fp = fopen(fname_bmp, "wb"))) {
    return;
  }

  fwrite(header, sizeof(unsigned char), 54, fp);
  fwrite(image, sizeof(unsigned char), (size_t)(int64_t)imageWidth * imageHeight * 4, fp);
  fclose(fp);
}

static Napi::Number Method(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  auto startTime = GetTimer();
  // Register fallback fonts. It should be called only once when the application is being initialized.
  std::vector<std::string> fallbackFontPaths = {};
  fallbackFontPaths.emplace_back("/data/github.com/libpag/resources/font/NotoSerifSC-Regular.otf");
  fallbackFontPaths.emplace_back("/data/github.com/libpag/resources/font/NotoColorEmoji.ttf");
  std::vector<int> ttcIndices(fallbackFontPaths.size());
  pag::PAGFont::SetFallbackFontPaths(fallbackFontPaths, ttcIndices);

  auto pagFile = ReplaceImageOrText();
  if (pagFile == nullptr) {
    printf("---pagFile is nullptr!!!\n");
    return Napi::Number::New(env, -1);
  }

  auto pagSurface = pag::PAGSurface::MakeOffscreen(pagFile->width(), pagFile->height());
  if (pagSurface == nullptr) {
    printf("---pagSurface is nullptr!!!\n");
    return Napi::Number::New(env, -1);
  }
  auto pagPlayer = new pag::PAGPlayer();
  pagPlayer->setSurface(pagSurface);
  pagPlayer->setComposition(pagFile);

  auto totalFrames = TimeToFrame(pagFile->duration(), pagFile->frameRate());
  auto currentFrame = 0;

  int bytesLength = pagFile->width() * pagFile->height() * 4;

  while (currentFrame <= totalFrames) {
    pagPlayer->setProgress(currentFrame * 1.0 / totalFrames);
    auto status = pagPlayer->flush();

    printf("---currentFrame:%d, flushStatus:%d \n", currentFrame, status);

    auto data = new uint8_t[bytesLength];
    pagSurface->readPixels(pag::ColorType::BGRA_8888, pag::AlphaType::Premultiplied, data,
                           pagFile->width() * 4);

    std::string imageName = std::to_string(currentFrame);

    BmpWrite(data, pagFile->width(), pagFile->height(), imageName.c_str());

    delete[] data;

    currentFrame++;
  }

  delete pagPlayer;

  printf("----timeCost--:%lld \n", GetTimer() - startTime);

  return Napi::Number::New(env, 0);
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "hello"), Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(hello, Init)