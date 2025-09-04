# 🚀 9주차 4팀 Pintos Project 1 - Threads

**팀원**: 이승준, 김형진, 조예은

## 📋 프로젝트 개요
이 저장소는 9주차 4팀의 Pintos Project 1 (Threads) 과제를 위한 협업 공간입니다.
각 팀원이 독립적으로 구현하며, 코드 리뷰를 통해 서로 학습하는 것을 목표로 합니다.

## 📁 폴더 구조
```
pintos-team/
├── seungjun/          # 이승준 작업 폴더
├── hyungjin/          # 김형진 작업 폴더
├── yeeun/             # 조예은 작업 폴더
├── shared/            # 공유폴더 (참고문서 등..)
└── README.md          # 프로젝트 설명
```

## 🔄 Git 워크플로우

### 1. 저장소 Fork 및 Clone
```bash
# 각자 메인 저장소를 Fork 한 후
git clone https://github.com/{본인아이디}/pintos-team.git
cd pintos-team
```
### 2. 브랜치 생성 및 작업
```bash
# 본인 이름으로 브랜치 생성
git checkout -b {이름}/{feature}
# 예시: git checkout -b seungjun/alarm-clock

# 본인 폴더에서 작업 후 커밋
cd seungjun/  # 본인 폴더로 이동
git add .
git commit -m "add: alarm clock 기능추가"
```
### 3. Push 및 Pull Request
```bash
# 본인 Fork에 Push
git push origin {이름}/alarm-clock

# GitHub에서 원본 저장소로 Pull Request 생성
# 제목 예시: "[승준] Alarm Clock 구현 완료"
```

### 📝 커밋 컨벤션
```bash
feat: 새로운 기능 추가
fix: 버그 수정
docs: 문서 수정
test: 테스트 추가/수정
refactor: 코드 리팩토링

예시:

feat: implement alarm clock in timer_sleep()
fix: resolve priority scheduling bug
test: add test cases for thread priority
```



---

# 📘 Docker기반 Pintos 개발 환경 구축 가이드 

이 문서는 **Windows**와 **macOS** 사용자가 Docker와 VSCode DevContainer 기능을 활용하여 Pintos OS 프로젝트를 빠르게 구축할 수 있도록 도와줍니다.

[**주의**]
* ubunbu:22.04 버전은 충분한 테스트와 검증이 되지 않았습니다. 이 점을 주의해서 사용하시기 바랍니다.

[**참고**] 
* pintos 도커 환경은 `64비트 기반 X86-64` 기반의 `ubuntu:22.04` 버전을 사용합니다.
   * kaist-pintos는 오리지널 pintos와 달리 64비트 환경을 지원합니다.
   * 이번 도커 환경은 ubuntu 22.04를 지원하여 vscode의 최신 버전에서 원격 연결이 안되는 문제를 해결하였습니다.
* pintos 도커 환경은 kaist-pintos에서 추천하는 qemu 에뮬레이터를 설치하고 사용합니다. 
* pintos 도커 환경은 9주차부터 13주차까지 같은 환경을 사용합니다. 이 기간동안 별도의 개발 환경을 제공하지 않습니다.
* 기존 도커 환경과 달리 `vscode`와 통합된 디버깅 환경(F5로 시작하는)을 제공하지 않습니다. 디버깅이 필요한 경우 `gdb`를 사용하세요. 
* vscode에서 터미널을 오픈하면 자동으로 `source /workspaces/pintos_22.04_lab_docker/pintos/activate`를 실행합니다.

---

## 1. Docker란 무엇인가요?

**Docker**는 애플리케이션을 어떤 컴퓨터에서든 **동일한 환경에서 실행**할 수 있게 도와주는 **가상화 플랫폼**입니다.  

Docker는 다음 구성요소로 이루어져 있습니다:

- **Docker Engine**: 컨테이너를 실행하는 핵심 서비스
- **Docker Image**: 컨테이너 생성에 사용되는 템플릿 (레시피 📃)
- **Docker Container**: 이미지를 기반으로 생성된 실제 실행 환경 (요리 🍜)

### ✅ AWS EC2와의 차이점

| 구분 | EC2 같은 VM | Docker 컨테이너 |
|------|-------------|-----------------|
| 실행 단위 | OS 포함 전체 | 애플리케이션 단위 |
| 실행 속도 | 느림 (수십 초 이상) | 매우 빠름 (거의 즉시) |
| 리소스 사용 | 무거움 | 가벼움 |

---

## 2. VSCode DevContainer란 무엇인가요?

**DevContainer**는 VSCode에서 Docker 컨테이너를 **개발 환경**처럼 사용할 수 있게 해주는 기능입니다.

- 코드를 실행하거나 디버깅할 때 **컨테이너 내부 환경에서 동작**
- 팀원 간 **환경 차이 없이 동일한 개발 환경 구성** 가능
- `.devcontainer` 폴더에 정의된 설정을 VSCode가 읽어 자동 구성

---

## 3. Docker Desktop 설치하기

1. Docker 공식 사이트에서 설치 파일 다운로드:  
   👉 [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)

2. 설치 후 Docker Desktop 실행  
   - Windows: Docker 아이콘이 트레이에 떠야 함  
   - macOS: 상단 메뉴바에 Docker 아이콘 확인

---

## 4. 프로젝트 파일 다운로드 (히스토리 없이)

터미널(CMD, PowerShell, zsh 등)에서 아래 명령어로 프로젝트 폴더만 내려받습니다:

```bash
git clone --depth=1 https://github.com/krafton-jungle/pintos_lab_docker.git
```

- `--depth=1` 옵션은 git commit 히스토리를 생략하고 **최신 파일만 가져옵니다.**

### 📂 다운로드 후 폴더 구조 설명

```
pintos_22.04_lab_docker/
├── .devcontainer/
│   ├── devcontainer.json      # VSCode에서 컨테이너 환경 설정
│   └── Dockerfile             # pintos 개발 환경 도커 이미지 정의
│
├── pintos
│   ├── threads                # 9주차 threads 프로젝트 폴더
│   ├── userprog               # 10-11주차 user program 프로젝트 폴더
│   └── vm                     # 12-13주차 virtual memory 프로젝트 폴더
│
└── README.md                  # 현재 문서
```
---

## 5. VSCode에서 해당 프로젝트 폴더 열기

1. VSCode를 실행
2. `파일 → 폴더 열기`로 방금 클론한 `pintos_22.04_lab_docker` 폴더를 선택

---

## 6. 개발 컨테이너: 컨테이너에서 열기

1. VSCode에서 `Ctrl+Shift+P` (Windows/Linux) 또는 `Cmd+Shift+P` (macOS)를 누릅니다.
2. 명령어 팔레트에서 `Dev Containers: Reopen in Container`를 선택합니다.
3. 이후 컨테이너가 자동으로 실행되고 빌드됩니다. 처음 컨테이너를 열면 빌드하는 시간이 오래걸릴 수 있습니다. 빌드 후, 프로젝트가 **컨테이너 안에서 실행됨**.

---

## 7. C 파일에 브레이크포인트 설정 후 디버깅 (F5)
pintos 랩에서는 vscode기반의 디버깅을 지원하지 않습니다. 

---
## 8. 새로운 Git 리포지토리에 Commit & Push 하기

금주 프로젝트를 개인 Git 리포와 같은 다른 리포지토리에 업로드하려면, 기존 Git 연결을 제거하고 새롭게 초기화해야 합니다.

### ✅ 완전히 새로운 Git 리포로 업로드하는 방법

아래 명령어를 순서대로 실행하세요:

```bash
rm -rf .git
git init
git remote add origin https://github.com/myusername/my-new-repo.git
git add .
git commit -m "Clean start"
git push -u origin main
```

### 📌 설명

- `rm -rf .git`: 기존 Git 기록과 연결을 완전히 삭제합니다.
- `git init`: 현재 폴더를 새로운 Git 리포지토리로 초기화합니다.
- `git remote add origin ...`: 새로운 리포지토리 주소를 origin으로 등록합니다.
- `git add .` 및 `git commit`: 모든 파일을 커밋합니다.
- `git push`: 새로운 리포에 최초 업로드(Push)합니다.

이 과정을 거치면 기존 리포와의 연결은 완전히 제거되고, **새로운 독립적인 프로젝트로 관리**할 수 있습니다.