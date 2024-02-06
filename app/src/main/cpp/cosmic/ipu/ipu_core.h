#pragma once

#include <common/types.h>
#include <ipu/decoder_fifo.h>
#include <ipu/data_matrix.h>
#include <mio/dma_ctrl.h>
namespace cosmic::ipu {
    enum PictureVDec : u8 {
        I = 1, P, B, D
    };
    struct IpuStatus {
        // When set, it treats the bitstream as MPEG-1 format; otherwise,
        // we are dealing with an MPEG-2 image
        bool isMpeg1;
        bool hasError;
        bool startCode;
        bool rst;
        u8 pictureCode;
    };
    enum FifoLayout {
        In,
        Out
    };

    class IpuMpeg2 {
    public:
        IpuMpeg2(std::shared_ptr<mio::DmaController>& direct);
        void resetDecoder();
        void update();

        static std::array<u32, 0x40> inverseScanZZ;
        static std::array<u32, 0x40> inverseScanAlternate;
        // A device or algorithmic function that performs quantization is called a quantizer.
        // An analog-to-digital converter is an example of a quantizer
        static std::array<u32, 0x20> quantizerLinear;
        static std::array<u32, 0x20> quantizerNonLinear;

        bool fifoIsEchoing(FifoLayout fifo);
        void issueACmd(u32 cmd);
    private:
        std::array<u8, 0x100> crCbMap;
        DataMatrix<u8, 0x4> ditherMtx;

        IpuStatus status;
        DecoderFifo in, out;
        std::shared_ptr<mio::DmaController> dmac;
    };

}
