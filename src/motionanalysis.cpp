#include "libmotioncapture/motionanalysis.h"

#include <mutex>
#include <unordered_map>
#include <thread>
#include "cortex.h"

#include <iostream> // TODO: Remove

#define RAD_TO_DEG 57.295779513082320876798154814105

namespace libmotioncapture {

    std::mutex mtx;
    sFrameOfData frameOfData = {0};

    const sFrameOfData &GetCurrentFrame() {
        static sFrameOfData frame = {0};
        {
            std::lock_guard<std::mutex> lck(mtx);
            frame = frameOfData;
        }
        return frame;
    }

    void ErrorMsgHandler(int iLevel, const char *szMsg) {
        const char *szLevel = nullptr;

        if (iLevel == VL_Debug) {
            szLevel = "Debug";
        } else if (iLevel == VL_Info) {
            szLevel = "Info";
        } else if (iLevel == VL_Warning) {
            szLevel = "Warning";
        } else if (iLevel == VL_Error) {
            szLevel = "Error";
        }

        printf("    %s: %s\n", szLevel, szMsg);
    }

    void DataHandler(sFrameOfData *pFrameOfData) {
        std::lock_guard<std::mutex> lck(mtx);
        frameOfData = *pFrameOfData;
    }

    class MotionCaptureMotionAnalysisImpl {
    public:
        std::string version = "0.0.0";
        sBodyDefs *pBodyDefs = nullptr;

        int lastFrame = 0;

        ~MotionCaptureMotionAnalysisImpl() {
            if (nullptr != pBodyDefs) {
                pBodyDefs = nullptr;
            }
        }
    };

    MotionCaptureMotionAnalysis::MotionCaptureMotionAnalysis(
            const std::string &hostname,
            unsigned short cortex_port = 1510,
            unsigned short multicast_port = 1001
        ) {
        pImpl = new MotionCaptureMotionAnalysisImpl();

        unsigned char SDK_Version[4];
        int retval;

        Cortex_SetVerbosityLevel(VL_Info);
        Cortex_GetSdkVersion(SDK_Version);
        pImpl->version = std::to_string(SDK_Version[1]) + "." + std::to_string(SDK_Version[2]) + "." +
                         std::to_string(SDK_Version[3]);

        Cortex_SetErrorMsgHandlerFunc(ErrorMsgHandler);
        Cortex_SetDataHandlerFunc(DataHandler);

        retval = Cortex_Initialize(hostname.c_str(), nullptr, cortex_port, multicast_port);

        if (retval != RC_Okay) {
            std::stringstream sstr;
            sstr << "Error: Unable to initialize ethernet communication";
            throw std::runtime_error(sstr.str());
        }
    }

    MotionCaptureMotionAnalysis::~MotionCaptureMotionAnalysis() {
        if (nullptr != pImpl) {
            delete pImpl;
            pImpl = nullptr;
        }
    }

    const std::string &MotionCaptureMotionAnalysis::version() const {
        return pImpl->version;
    }

    void MotionCaptureMotionAnalysis::waitForNextFrame() {
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();

        int frameNo;
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            frameNo = GetCurrentFrame().iFrame;
        } while (frameNo == pImpl->lastFrame);
        pImpl->lastFrame = frameNo;
        lastTime = now;
    }

    const std::map<std::string, RigidBody> &MotionCaptureMotionAnalysis::rigidBodies() const {
        // TODO: Implement an appropriate pose estimation algorithm
        rigidBodies_.clear();

        sFrameOfData pFrameOfData = GetCurrentFrame();

        for (int iBody = 0; iBody < pFrameOfData.nBodies; iBody++) {
            sBodyData *Body = &pFrameOfData.BodyData[iBody];

            Eigen::Vector3f position(0, 0, 0);
            Eigen::Quaternionf rotation = Eigen::Quaternionf::Identity();

            if (Body->nSegments > 0) {
                position = Eigen::Vector3f(Body->Segments[0][0], Body->Segments[0][1], Body->Segments[0][2]);
                Eigen::Vector3f rpy = Eigen::Vector3f(
                    -Body->Segments[0][3] / RAD_TO_DEG,
                    -Body->Segments[0][4] / RAD_TO_DEG,
                    Body->Segments[0][5] / RAD_TO_DEG
                );
                rotation = Eigen::AngleAxisf(rpy[0], Eigen::Vector3f::UnitX())
                         * Eigen::AngleAxisf(rpy[1], Eigen::Vector3f::UnitY())
                         * Eigen::AngleAxisf(rpy[2], Eigen::Vector3f::UnitZ());
            } else if (Body->nMarkers > 0) {
                position = Eigen::Vector3f(Body->Markers[0][0], Body->Markers[0][1], Body->Markers[0][2]);
            } else {
                throw std::invalid_argument("Body has no markers or segments");
            }

            char *bodyName = Body->szName;

            rigidBodies_.emplace(bodyName, RigidBody(bodyName, position, rotation));
        }

        return rigidBodies_;
    }

    RigidBody MotionCaptureMotionAnalysis::rigidBodyByName(const std::string &name) const {
        sFrameOfData pFrameOfData = GetCurrentFrame();

        for (int iBody = 0; iBody < pFrameOfData.nBodies; iBody++) {
            sBodyData *Body = &pFrameOfData.BodyData[iBody];

            if (Body->szName == name) {
                float centroid[3] = {0, 0, 0};

                for (int iMarker = 0; iMarker < Body->nMarkers; iMarker++) {
                    centroid[0] += Body->Markers[iMarker][0];
                    centroid[1] += Body->Markers[iMarker][1];
                    centroid[2] += Body->Markers[iMarker][2];
                }

                centroid[0] /= (float) Body->nMarkers;
                centroid[1] /= (float) Body->nMarkers;
                centroid[2] /= (float) Body->nMarkers;

                Eigen::Vector3f position(centroid[0], centroid[1], centroid[2]);
                Eigen::Quaternionf rotation = Eigen::Quaternionf::Identity();

                return RigidBody(name, position, rotation);
            }
        }
        throw std::runtime_error("Rigid body not found");
    }

} // namespace libmotioncapture