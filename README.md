# kmdf-example
<img src="https://img.shields.io/badge/c-064F8C?style=for-the-badge&logo=c&logoColor=white">

학습용 Windows Kernel-Mode Driver Framework (KMDF) 예제 모음.

- **kmdf-echo**: 
  DeviceIoControl()을 통해 문자열을 전달받는 가상 KMDF 디바이스 드라이버

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
