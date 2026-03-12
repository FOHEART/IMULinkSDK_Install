#ifndef IMULINKSDK_TYPESDEF_H
#define IMULINKSDK_TYPESDEF_H

#include <string>

/**
 * @file imulinksdk_typedefs.h
 * @brief KHS53 skeleton indices and helper utilities. / KHS53 骨架索引与辅助工具。
 */

enum KHS53_SkeletonIndex_s
{
    /** @brief Pelvis / 骨盆 */
    KHS53_Pelvis,

    /** @brief L5 (5th lumbar vertebra) / 第5腰椎 */
    KHS53_L5,

    /** @brief L3 (3rd lumbar vertebra) / 第3腰椎 */
    KHS53_L3,

    /** @brief T12 (12th thoracic vertebra) / 第12胸椎 */
    KHS53_T12,

    /** @brief T8 (8th thoracic vertebra) / 第8胸椎 */
    KHS53_T8,

    /** @brief Neck / 颈部 */
    KHS53_Neck,

    /** @brief Head / 头部 */
    KHS53_Head,

    /** @brief Right shoulder / 右肩 */
    KHS53_RightShoulder,

    /** @brief Right upper arm / 右上臂 */
    KHS53_RightUpperArm,

    /** @brief Right forearm / 右前臂 */
    KHS53_RightForeArm,

    /** @brief Right hand / 右手 */
    KHS53_RightHand,

    /** @brief Right thumb, proximal phalanx (1) / 右拇指 第一节 */
    KHS53_RightHandThumb1,

    /** @brief Right thumb, middle phalanx (2) / 右拇指 第二节 */
    KHS53_RightHandThumb2,

    /** @brief Right thumb, distal phalanx (3) / 右拇指 第三节 */
    KHS53_RightHandThumb3,

    /** @brief Right index finger, proximal phalanx (1) / 右食指 第一节 */
    KHS53_RightHandIndex1,

    /** @brief Right index finger, middle phalanx (2) / 右食指 第二节 */
    KHS53_RightHandIndex2,

    /** @brief Right index finger, distal phalanx (3) / 右食指 第三节 */
    KHS53_RightHandIndex3,

    /** @brief Right middle finger, proximal phalanx (1) / 右中指 第一节 */
    KHS53_RightHandMiddle1,

    /** @brief Right middle finger, middle phalanx (2) / 右中指 第二节 */
    KHS53_RightHandMiddle2,

    /** @brief Right middle finger, distal phalanx (3) / 右中指 第三节 */
    KHS53_RightHandMiddle3,

    /** @brief Right ring finger, proximal phalanx (1) / 右无名指 第一节 */
    KHS53_RightHandRing1,

    /** @brief Right ring finger, middle phalanx (2) / 右无名指 第二节 */
    KHS53_RightHandRing2,

    /** @brief Right ring finger, distal phalanx (3) / 右无名指 第三节 */
    KHS53_RightHandRing3,

    /** @brief Right pinky, proximal phalanx (1) / 右小指 第一节 */
    KHS53_RightHandPinky1,

    /** @brief Right pinky, middle phalanx (2) / 右小指 第二节 */
    KHS53_RightHandPinky2,

    /** @brief Right pinky, distal phalanx (3) / 右小指 第三节 */
    KHS53_RightHandPinky3,

    /** @brief Left shoulder / 左肩 */
    KHS53_LeftShoulder,

    /** @brief Left upper arm / 左上臂 */
    KHS53_LeftUpperArm,

    /** @brief Left forearm / 左前臂 */
    KHS53_LeftForeArm,

    /** @brief Left hand / 左手 */
    KHS53_LeftHand,

    /** @brief Left thumb, proximal phalanx (1) / 左拇指 第一节 */
    KHS53_LeftHandThumb1,

    /** @brief Left thumb, middle phalanx (2) / 左拇指 第二节 */
    KHS53_LeftHandThumb2,

    /** @brief Left thumb, distal phalanx (3) / 左拇指 第三节 */
    KHS53_LeftHandThumb3,

    /** @brief Left index finger, proximal phalanx (1) / 左食指 第一节 */
    KHS53_LeftHandIndex1,

    /** @brief Left index finger, middle phalanx (2) / 左食指 第二节 */
    KHS53_LeftHandIndex2,

    /** @brief Left index finger, distal phalanx (3) / 左食指 第三节 */
    KHS53_LeftHandIndex3,

    /** @brief Left middle finger, proximal phalanx (1) / 左中指 第一节 */
    KHS53_LeftHandMiddle1,

    /** @brief Left middle finger, middle phalanx (2) / 左中指 第二节 */
    KHS53_LeftHandMiddle2,

    /** @brief Left middle finger, distal phalanx (3) / 左中指 第三节 */
    KHS53_LeftHandMiddle3,

    /** @brief Left ring finger, proximal phalanx (1) / 左无名指 第一节 */
    KHS53_LeftHandRing1,

    /** @brief Left ring finger, middle phalanx (2) / 左无名指 第二节 */
    KHS53_LeftHandRing2,

    /** @brief Left ring finger, distal phalanx (3) / 左无名指 第三节 */
    KHS53_LeftHandRing3,

    /** @brief Left pinky, proximal phalanx (1) / 左小指 第一节 */
    KHS53_LeftHandPinky1,

    /** @brief Left pinky, middle phalanx (2) / 左小指 第二节 */
    KHS53_LeftHandPinky2,

    /** @brief Left pinky, distal phalanx (3) / 左小指 第三节 */
    KHS53_LeftHandPinky3,

    /** @brief Right upper leg / 右大腿 */
    KHS53_RightUpperLeg,

    /** @brief Right lower leg / 右小腿 */
    KHS53_RightLowerLeg,

    /** @brief Right foot / 右脚 */
    KHS53_RightFoot,

    /** @brief Right toe / 右脚趾 */
    KHS53_RightToe,

    /** @brief Left upper leg / 左大腿 */
    KHS53_LeftUpperLeg,

    /** @brief Left lower leg / 左小腿 */
    KHS53_LeftLowerLeg,

    /** @brief Left foot / 左脚 */
    KHS53_LeftFoot,

    /** @brief Left toe / 左脚趾 */
    KHS53_LeftToe,

    /** @brief Count sentinel / 总数 标记 */
    KHS53_Count
};

/** @brief Typedef for enum KHS53_SkeletonIndex. / KHS53_SkeletonIndex 的枚举别名。 */
typedef enum KHS53_SkeletonIndex_s KHS53_SkeletonIndex;

/**
 * @brief Return a null-terminated string name for a given KHS53_SkeletonIndex.
 * @brief-cn 返回给定 KHS53_SkeletonIndex 的以 null 结尾的名称字符串，用于打印/日志。
 * @param idx The skeleton index. / 骨架索引。
 * @return const char* C-string name for the index. / 索引的 C 字符串名称。
 */
static inline const char* KHS53_SkeletonIndexToString(KHS53_SkeletonIndex idx)
{
    switch (idx)
    {
        case KHS53_Pelvis: return "KHS53_Pelvis";
        case KHS53_L5: return "KHS53_L5";
        case KHS53_L3: return "KHS53_L3";
        case KHS53_T12: return "KHS53_T12";
        case KHS53_T8: return "KHS53_T8";
        case KHS53_Neck: return "KHS53_Neck";
        case KHS53_Head: return "KHS53_Head";

        case KHS53_RightShoulder: return "KHS53_RightShoulder";
        case KHS53_RightUpperArm: return "KHS53_RightUpperArm";
        case KHS53_RightForeArm: return "KHS53_RightForeArm";
        case KHS53_RightHand: return "KHS53_RightHand";

        case KHS53_RightHandThumb1: return "KHS53_RightHandThumb1";
        case KHS53_RightHandThumb2: return "KHS53_RightHandThumb2";
        case KHS53_RightHandThumb3: return "KHS53_RightHandThumb3";
        case KHS53_RightHandIndex1: return "KHS53_RightHandIndex1";
        case KHS53_RightHandIndex2: return "KHS53_RightHandIndex2";
        case KHS53_RightHandIndex3: return "KHS53_RightHandIndex3";
        case KHS53_RightHandMiddle1: return "KHS53_RightHandMiddle1";
        case KHS53_RightHandMiddle2: return "KHS53_RightHandMiddle2";
        case KHS53_RightHandMiddle3: return "KHS53_RightHandMiddle3";
        case KHS53_RightHandRing1: return "KHS53_RightHandRing1";
        case KHS53_RightHandRing2: return "KHS53_RightHandRing2";
        case KHS53_RightHandRing3: return "KHS53_RightHandRing3";
        case KHS53_RightHandPinky1: return "KHS53_RightHandPinky1";
        case KHS53_RightHandPinky2: return "KHS53_RightHandPinky2";
        case KHS53_RightHandPinky3: return "KHS53_RightHandPinky3";

        case KHS53_LeftShoulder: return "KHS53_LeftShoulder";
        case KHS53_LeftUpperArm: return "KHS53_LeftUpperArm";
        case KHS53_LeftForeArm: return "KHS53_LeftForeArm";
        case KHS53_LeftHand: return "KHS53_LeftHand";

        case KHS53_LeftHandThumb1: return "KHS53_LeftHandThumb1";
        case KHS53_LeftHandThumb2: return "KHS53_LeftHandThumb2";
        case KHS53_LeftHandThumb3: return "KHS53_LeftHandThumb3";
        case KHS53_LeftHandIndex1: return "KHS53_LeftHandIndex1";
        case KHS53_LeftHandIndex2: return "KHS53_LeftHandIndex2";
        case KHS53_LeftHandIndex3: return "KHS53_LeftHandIndex3";
        case KHS53_LeftHandMiddle1: return "KHS53_LeftHandMiddle1";
        case KHS53_LeftHandMiddle2: return "KHS53_LeftHandMiddle2";
        case KHS53_LeftHandMiddle3: return "KHS53_LeftHandMiddle3";
        case KHS53_LeftHandRing1: return "KHS53_LeftHandRing1";
        case KHS53_LeftHandRing2: return "KHS53_LeftHandRing2";
        case KHS53_LeftHandRing3: return "KHS53_LeftHandRing3";
        case KHS53_LeftHandPinky1: return "KHS53_LeftHandPinky1";
        case KHS53_LeftHandPinky2: return "KHS53_LeftHandPinky2";
        case KHS53_LeftHandPinky3: return "KHS53_LeftHandPinky3";

        case KHS53_RightUpperLeg: return "KHS53_RightUpperLeg";
        case KHS53_RightLowerLeg: return "KHS53_RightLowerLeg";
        case KHS53_RightFoot: return "KHS53_RightFoot";
        case KHS53_RightToe: return "KHS53_RightToe";
        case KHS53_LeftUpperLeg: return "KHS53_LeftUpperLeg";
        case KHS53_LeftLowerLeg: return "KHS53_LeftLowerLeg";
        case KHS53_LeftFoot: return "KHS53_LeftFoot";
        case KHS53_LeftToe: return "KHS53_LeftToe";

        case KHS53_Count: return "KHS53_Count";
        default: return "KHS53_Unknown";
    }
}

/**
 * @brief std::string helper that wraps the const char* version to avoid duplication.
 * @brief-cn std::string 辅助函数：包装 const char* 版本以避免重复。
 * @param idx The skeleton index. / 骨架索引。
 * @return std::string Name as std::string. / 以 std::string 返回名称。
 */
static inline std::string KHS53_SkeletonIndexToStdString(KHS53_SkeletonIndex idx)
{
    return std::string(KHS53_SkeletonIndexToString(idx));
}

#endif /* IMULINKSDK_TYPESDEF_H */
