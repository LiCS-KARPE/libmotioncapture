import motioncapture
import argparse

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("type")
    parser.add_argument("hostname")
    parser.add_argument("--cortex_port", default="1510")
    parser.add_argument("--multicast_port", default="1511")
    args = parser.parse_args()

    cfg = args.__dict__

    mc = motioncapture.connect(args.type, cfg)
    while True:
        mc.waitForNextFrame()
        for name, obj in mc.rigidBodies.items():
            print("[%s] x: %.2f, y: %.2f, z: %.2f, qx: %.2f, qy: %.2f, qz: %.2f, qw: %.2f" % (name, obj.position[0], obj.position[1], obj.position[2], obj.rotation.x, obj.rotation.y, obj.rotation.z, obj.rotation.w))
