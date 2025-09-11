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
