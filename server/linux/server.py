#!/usr/bin/python3

import psutil
import time
import socket
import hid
import signal
import atexit
import sys


DISK_USAGE_PATH = "/"


class Info:
    __instance = None
    _USB_VID = 0xcafe

    def __new__(cls):
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)
        return cls.__instance

    def __init__(self) -> None:
        self.__prev_upload = 0
        self.__prev_download = 0
        self.__first_network_call = True

        usbs = hid.enumerate(self._USB_VID)
        if(not len(usbs)):
            raise RuntimeError(f"Could not find device with VID: \
{self.USB_VID}")

        usb_device = hid.Device(usbs[0]['vendor_id'], usbs[0]['product_id'])
        if(not usb_device):
            raise RuntimeError("Unable to open device")
        self._usb_device = usb_device
        self.__temp_zone = open("/sys/class/thermal/thermal_zone0/temp", "r")
        atexit.register(self.cleanup)

    def cleanup(self, signal_received=None, frame=None):
        if self._usb_device is not None:
            self._usb_device.close()
        self.__temp_zone.close()
        if signal_received:
            sys.exit()

    @property
    def KIBI(self) -> int:
        return 1024

    @property
    def MEBI(self) -> int:
        return 1048576

    @property
    def GIBI(self) -> int:
        return 1073741824

    @property
    def TEBI(self) -> int:
        return 1099511627776

    def cpu_percentage(self) -> int:
        return psutil.cpu_percent()

    def mem_percentage(self) -> int:
        return psutil.virtual_memory().percent

    def disk_percentage(self, disk_location) -> int:
        return psutil.disk_usage(disk_location).percent

    def network_usage(self) -> tuple:
        current = psutil.net_io_counters()

        if current is None:
            return ("0 B", "0 B", False)

        if self.__first_network_call:
            self.__prev_upload = current.bytes_sent
            self.__prev_download = current.bytes_recv
            self.__first_network_call = False
            return ("0 B", "0 B", True)

        up_speed = current.bytes_sent - self.__prev_upload
        down_speed = current.bytes_recv - self.__prev_download
        self.__prev_upload = current.bytes_sent
        self.__prev_download = current.bytes_recv

        return (self.format_unit(up_speed), self.format_unit(down_speed), True)

    def format_unit(self, byt) -> str:
        if byt < self.KIBI:
            return f"{round(byt)} B"
        elif byt < self.MEBI:
            return f"{round(byt/self.KIBI, 1)} KB"
        elif byt < self.GIBI:
            return f"{round(byt/self.MEBI, 1)} MB"
        elif byt < self.TEBI:
            return f"{round(byt/self.GIBI, 1)} GB"
        else:
            return f"{round(byt/self.TEBI, 1)} TB"

    def is_internet_available(self) -> bool:
        try:
            socket.setdefaulttimeout(3)
            socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect(
                ("8.8.8.8", 53))
            return True
        except socket.error:
            return False

    def is_overheating(self) -> bool:
        temp = self.__temp_zone.readline()
        self.__temp_zone.seek(0)
        if int(temp) > 60000:
            return True
        else:
            return False

    def send_data(self, dt) -> None:
        self._usb_device.write(dt)


if __name__ == "__main__":
    info = Info()
    signal.signal(signal.SIGINT, info.cleanup)
    internet_available = False
    internet_status_refresh_rate = 1 * 60  # 1 minute
    prev_time = time.time()

    while(True):
        net = info.network_usage()
        current_time = time.time()

        if current_time - prev_time > internet_status_refresh_rate and net[2]:
            internet_available = info.is_internet_available()
            prev_time = current_time

        if net[2]:
            str_data = "{}%;{}%;{}%;DL: {}/s\nUL: {}/s;{};{};".format(
                info.cpu_percentage(),
                info.mem_percentage(),
                info.disk_percentage(DISK_USAGE_PATH),
                net[1],
                net[0],
                int(internet_available),
                int(info.is_overheating())
            )
        else:
            str_data = "{}%;{}%;{}%;Interface\nN\\A;0;{};".format(
                info.cpu_percentage(),
                info.mem_percentage(),
                info.disk_percentage(DISK_USAGE_PATH),
                int(info.is_overheating())
            )
        info.send_data(str_data.encode("utf-8"))
        time.sleep(1)
