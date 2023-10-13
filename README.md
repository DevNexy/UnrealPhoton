## UnrealPhoton(Cloud, Chat, Voice) - 언리얼 포톤(클라우드, 챗, 보이스)
## Developed using UE5 and Photon SDK 

![image](https://github.com/DevNexy/UnrealPhoton/assets/92451281/c4cd10ea-a005-45f1-a5a3-2cf6e68ed78e)
[PhotonChat 시연](https://youtu.be/f85KT7glvdM)

---
<요약>
- Photon SDK를 활용한 채널 별 텍스트 채팅, 보이스 채팅 기능 구현
- 개인 프로젝트   
[Unreal Photon SDK](https://www.photonengine.com/sdks#realtime-sdkrealtimewindows)   
[Photon API Doc](https://doc-api.photonengine.com/en/cpp/current/index.html)

---
<콘텐츠 소개>
- 게임에서 필수적인 텍스트 채팅과 보이스 채팅
- 채널 별로 채팅 가능

---
<오류 해결>
- 채널 정렬 오류 문제
  - 버그 현상 : 코드에서 채널을 인덱스 형식으로 받아와서 채널에 접근 및 출력했는데, 기존에 받은 메세지를 출력할 때 순서가 달라지는 현상 발생
  - 원인 : 채널을 인덱스 형식으로 구별하면 순서에 의존하기 때문에 문제가 발생
  - 해결 방법 : 채널의 순서에 따라 메시지를 올바르게 출력하기 위해 채널을 이름 또는 다른 고유한 식별자로 구별 하였습니다.
- SDK 및 라이브러리 설정 문제
  - 버그 현상 : SDK 및 라이브러리 설정 문제로 인한 링커 오류 발생
  - 해결 방법 : Build.cs 파일에서 포톤 경로 수정 및 라이브러리 추가 (코드 참고)
