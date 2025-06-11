# Snake Game (C++ & NCurses)

NCurses 라이브러리를 사용한 Snake Game 구현 프로젝트입니다.

## 단계별 구현 목표

### 1단계: 기본 맵 구현 (현재 완료)
- [x] 21x21 크기의 기본 맵 구현
- [x] Immune Wall과 일반 Wall 구분 구현
- [x] Snake의 머리와 몸통 표시 구현
- [x] NCurses를 이용한 화면 출력
- [x] 단위 테스트 구현

### 2단계: Snake 이동 구현 (현재 완료)
- [x] Snake 이동 메커니즘 구현
  - [x] 일정 시간 간격으로 이동 (tick) - 200ms 간격
  - [x] 키보드 입력에 따른 방향 전환 (방향키 사용)
  - [x] 현재 진행 방향의 반대 방향 이동 제한
- [x] 충돌 감지 구현
  - [x] 자기 자신과의 충돌 감지
  - [x] 벽과의 충돌 감지 (Immune Wall, Wall 모두)
- [x] Snake 이동 관련 단위 테스트 구현

### 3단계: 아이템 시스템 구현
- [ ] Growth Item 구현
  - [ ] 랜덤 위치에 Growth Item 생성
  - [ ] Growth Item 획득 시 Snake 길이 증가
- [ ] Poison Item 구현
  - [ ] 랜덤 위치에 Poison Item 생성
  - [ ] Poison Item 획득 시 Snake 길이 감소
  - [ ] Snake 길이 3 미만 시 게임 오버
- [ ] 아이템 관리 시스템 구현
  - [ ] 최대 3개의 아이템 동시 존재
  - [ ] 아이템 지속 시간 (5초) 구현
  - [ ] 미획득 아이템 자동 재생성
- [ ] 아이템 시스템 단위 테스트 구현

### 4단계: Gate 시스템 구현
- [ ] Gate 생성 메커니즘 구현
  - [ ] Wall에만 Gate 생성 가능
  - [ ] 한 쌍의 Gate만 존재
  - [ ] Gate 중복 생성 방지
- [ ] Gate 이동 로직 구현
  - [ ] 입구 Gate에서 출구 Gate로 이동
  - [ ] Gate 출구 방향 우선순위 구현
    - [ ] 외곽 벽 Gate: 안쪽 방향으로만 진출
    - [ ] 내부 벽 Gate: 진입 방향 우선
- [ ] Gate 시스템 단위 테스트 구현

### 5단계: 점수 및 미션 시스템 구현
- [ ] 점수 시스템 구현
  - [ ] 현재/최대 길이 기록
  - [ ] Growth Item 획득 횟수
  - [ ] Poison Item 획득 횟수
  - [ ] Gate 사용 횟수
- [ ] 미션 시스템 구현
  - [ ] 목표 길이 달성
  - [ ] Growth Item 획득 목표
  - [ ] Poison Item 획득 목표
  - [ ] Gate 사용 목표
- [ ] 스테이지 시스템 구현
  - [ ] 최소 4개의 서로 다른 맵
  - [ ] 미션 완료 시 다음 스테이지로 진행
- [ ] 점수/미션 시스템 단위 테스트 구현

## 현재 구현된 기능

### 맵 표시
- 21x21 크기의 기본 맵
- Immune Wall (*): 맵 테두리를 둘러싸는 벽
- Wall (#): 일반 벽
- Snake Head (@): 뱀의 머리
- Snake Body (o): 뱀의 몸통
- 빈 공간 ( ): 이동 가능한 공간

### Snake 이동 시스템
- 방향키를 이용한 Snake 조작 (↑↓←→)
- 200ms 간격의 자동 이동 (tick 기반)
- 반대 방향 이동 제한 (게임 오버 방지)
- Snake 성장 기능
- 실시간 키보드 입력 처리 (non-blocking)

### 충돌 감지 시스템
- 벽과의 충돌 감지 (Immune Wall, Wall)
- 자기 자신과의 충돌 감지
- 맵 경계 충돌 감지
- 충돌 시 게임 오버 처리

### 게임 루프
- NCurses 기반 실시간 게임 루프
- 게임 오버 화면 표시
- 'q' 키로 게임 종료

### 테스트
- Google Test를 사용한 단위 테스트 구현
- GameMap 테스트 (5개 테스트)
- Snake 테스트 (7개 테스트)
- Game 테스트 (5개 테스트)
- 총 17개의 테스트 케이스 모두 통과

## 빌드 방법

### 필수 요구사항
- CMake (3.10 이상)
- C++ 컴파일러 (C++17 지원)
- NCurses 라이브러리
- Google Test (자동으로 다운로드됨)

### 빌드 명령어
```bash
mkdir build
cd build
cmake ..
make
```

### 실행 방법
기본 맵 표시 프로그램:
```bash
./snake_game
```

완전한 Snake 게임 (2단계 완료):
```bash
./snake_game_v2
```

테스트 실행:
```bash
./game_map_test    # GameMap 테스트
./snake_test       # Snake 테스트
./game_test        # Game 테스트
```

### 게임 조작법
- ↑↓←→ (방향키): Snake 이동
- q 또는 Q: 게임 종료

## 프로젝트 구조
- `GameMap.hpp`: 맵 클래스 헤더
- `GameMap.cpp`: 맵 클래스 구현
- `GameMapTest.cpp`: 맵 클래스 테스트
- `main.cpp`: 메인 프로그램
- `CMakeLists.txt`: CMake 빌드 설정

## 향후 구현 예정 기능
- Snake 이동 구현
- Growth Item (+) 구현
- Poison Item (-) 구현
- Gate 시스템 구현
- 점수 시스템 구현
- 미션 시스템 구현 