# kmdf-example
<img src="https://img.shields.io/badge/c-064F8C?style=for-the-badge&logo=c&logoColor=white">

<p align="center">
<img width="500" alt="Device Manager" src="https://github.com/user-attachments/assets/02e799e8-c055-4c78-931d-f5a764457f66" />
</p>

학습용 Windows Kernel-Mode Driver Framework (KMDF) 예제 모음.

- **kmdf-echo**: 
  DeviceIoControl()을 통해 문자열을 전달받는 가상 KMDF 디바이스 드라이버
- **storport-ramdisk**:
  Storport Virtual Miniport Driver (VMiniport)로 구현된 램 디스크 드라이버
- **remote-keyboard**:
  원격으로 키보드 입력을 받는 VHF (Virtual HID Framework) 드라이버
  
## Prerequisites
- Windows 11 24H2
- Visual Studio 2022 (Desktop development with C++)
- Windows Driver Kit / Windows SDK
> [!NOTE]
>**Enterprise WDK (EWDK)를 사용하면 쉽게 빌드 환경을 구축할 수 있습니다.**
>
> Visual Studio, WDK를 별도로 설치하는 대신에 드라이버 빌드를 위해 독립적인 커멘드라인 환경을 제공하는 EWDK를 사용할 수 있습니다.
>
> 
> 자세한 사항은 아래 Microsoft Learn 페이지를 참조하십시오:
> 
> https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk

## Running the Samples
### 테스트 서명 모드 사용하기
다음 명령을 관리자 권한의 명령 프롬프트에서 실행하여 테스트 서명 코드를 사용하도록 설정합니다.

변경 내용이 적용되려면 구성을 변경한 후 컴퓨터를 다시 시작해야 합니다.
```
Bcdedit.exe -set TESTSIGNING ON
```
데스크탑 좌측 하단에 Test Mode와 Windows 빌드 정보가 담긴 워터마크가 표시되는 것을 확인합니다.
- - -
### 드라이버 설치하기
Device Manager에서 Action->Add Driver 메뉴를 클릭하고 적절하게 컴파일한 드라이버 샘플의 .inf 파일이 존재하는 폴더를 선택합니다.

서명되지 않은 드라이버 경고 창이 뜨면 "Install this driver software anyway" 옵션을 선택하여 계속 진행합니다.

<p align="center">
<img width="380" alt="Windows Security" src="https://github.com/user-attachments/assets/bd1cfb5f-b521-4e4f-add0-46cdb740450d" />
</p>

- - -

### 장치 추가하기
드라이버 샘플은 실제 PnP 장치를 위한 드라이버가 아님으로 Device Manager에서 수동으로 장치를 추가해야 합니다.

Device Manager에서 최상위 루트 객체(데스크탑)을 선택하고 Action->Add Legacy Hardware 메뉴를 클릭합니다.
<p align="center">
  <img width="380" alt="Device Manager" src="https://github.com/user-attachments/assets/55e0997a-bf4c-4388-b6f3-30501742b88f" />
</p>

"Install the hardware that I manually select from a list" 옵션을 선택합니다.

<p align="center">
<img width="380" alt="Driver Manager" src="https://github.com/user-attachments/assets/87230891-080f-48ad-9ee4-484849ca6944" />
</p>

설치하려는 드라이버를 리스트에 찾아 설치합니다.

- - -
### WinDbg에서 드라이버 디버깅 메세지 읽기
WinDbg로 드라이버 디버깅 메세지를 읽으려면 대상 컴퓨터에 적절한 컴포넌트 필터 마스크 값을 설정해야 합니다.

WinDbg에서 대상 컴퓨터를 Break 하고 다음 명령을 입력합니다:
```
ed nt!Kd_IHVDRIVER_Mask 0xF
```

## Samples
### kmdf-echo
DeviceIoControl()을 통해 문자열을 전달받는 가상 KMDF 디바이스 드라이버

- KMDF 드라이버 기본 골격 (DriverEntry(), DeviceAdd(), 기본 I/O 큐)
- DeviceIoControl()을 통한 사용자 모드 앱과의 통신
- 커널모드 함수를 이용한 디스크 I/O

솔루션의 kmdf-echo-driver 드라이버를 설치하고 장치를 추가합니다.

<p align="center">
  <img width="500" alt="Screenshot 2025-09-13 002038" src="https://github.com/user-attachments/assets/00536693-edb0-40cb-a44f-2c512be86824" />
</p>

솔루션의 kmdf-echo-app을 빌드하여 실행하고 드라이버와 연결합니다. (Run as Administrator)

<p align="center">
  <img width="500" alt="Screenshot 2025-09-13 002310" src="https://github.com/user-attachments/assets/3b894a99-1ef9-4f19-bd8b-95d1cf86742f" />
</p>

C:\kmdf-echo-driver.logs 파일 또는 WinDbg 명령 창을 확인합니다. 

<p align="center">
  <img width="500" height="700" alt="image" src="https://github.com/user-attachments/assets/9c59a390-e2a9-48b4-aa2d-ec414532ef57" />
</p>

- - -

### storport-ramdisk

Storport Virtual Miniport Driver (VMiniport)로 구현된 램 디스크 드라이버

- Storport 가상 미니포트 드라이버 구조 구현
- SCSI 명령 처리 (READ, WRITE, REPORT LUNS, INQUERY, READ CAPACITY)
- HW_PASSIVE_INITIALIZE_ROUTINE에서 논페이지드 풀 메모리를 할당받아 해당 영역을 디스크로써 사용합니다.
- SCSI VPD 페이지 (0x80, 0xB1) 지원

>[!NOTE]
>**로그를 출력하려면 DBG 매크로 상수를 1로 선언합니다.**
>
>이것은 I/O 성능을 크게 저하시키기 때문에 권장하지 않습니다.

솔루션의 storport-ramdisk 드라이버를 설치하고 장치를 추가합니다.

<p align="center">
  <img width="500" alt="image" src="https://github.com/user-attachments/assets/c617b528-7535-4bde-99e4-aa0f7521885e" />
</p>

Disk Manager에서 가상 디스크 장치를 초기화하고 파티션을 구성합니다.

<p align="center">
  <img width="500" alt="image" src="https://github.com/user-attachments/assets/d27700a1-9548-4ccc-abd3-8e0c892bc19d" />
</p>

<p align="center">
  <img width="500" alt="Screenshot 2025-09-12 214728" src="https://github.com/user-attachments/assets/81b7bcaf-415c-42e5-850d-e01b95fdd486" />
</p>

디버그 매크로 상수를 설정하고 WinDbg 명령 창을 참조합니다.
<p align="center">
  <img width="500" alt="Screenshot 2025-09-12 204413" src="https://github.com/user-attachments/assets/fe4d2ce9-e6fc-4f35-8a46-945ef3232de4" />
</p>

- - -

### remote-keyboard

원격으로 키보드 입력을 받는 VHF (Virtual HID Framework) 드라이버

- WSK (Winsock Kernel)을 통한 UDP Listening
- VHF (Virtual HID Framework)를 통한 가상 HID 장치 에뮬레이션
- HID Device Boot Protocol 처리 (6키 롤오버)
- Win32 Hook을 이용한 KeyboardProc 후킹 (C# 기반 전송 클라이언트)

대상 장치에 솔루션의 remote-keyboard-driver 드라이버를 설치하고 장치를 추가합니다.

<p align="center">
  <img width="500" alt="Screenshot 2025-09-21 214843" src="https://github.com/user-attachments/assets/1b36ab8c-ed7e-43b1-aa42-859a35320c63" />
</p>

솔루션의 kmdf-echo-app을 빌드하여 실행하고 드라이버와 연결합니다. (대상 장치의 포트는 UDP/2381 입니다.)

<p align="center">
  <img width="500" alt="Screenshot 2025-09-21 212152" src="https://github.com/user-attachments/assets/3583221b-27e6-47ef-89f8-247723cc8e90" />
</p>

이제 클라이언트의 키 입력이 대상 장치에서 에뮬레이션 됩니다. 디버그 매크로 상수를 설정하고 WinDbg 명령 창을 참조합니다.

<p align="center">
  <img width="500" alt="Screenshot 2025-09-21 215535" src="https://github.com/user-attachments/assets/d4cd6262-e37f-48a8-a2a4-3c29cd2431a8" />
</p>
