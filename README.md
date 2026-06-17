# Mini Student Shell

C로 만든 학생 성적 관리 셸 프로그램이다. 학생 정보(학번, 이름, 점수)를 연결 리스트로 관리하고 CSV 파일로 저장한다. 모든 기능을 쓸 수 있는 관리자용(`admin_shell`)과 조회만 가능한 클라이언트용(`client_shell`)으로 나뉜다.

## 빌드

```bash
make          # admin_shell, client_shell 둘 다 빌드
make admin    # admin_shell만 빌드 (-DADMIN_MODE)
make client   # client_shell만 빌드 (-DCLIENT_MODE)
make clean    # 빌드 결과물 삭제
```

## 실행

```bash
./admin_shell students.csv     # 관리자 대화형 모드
./client_shell students.csv    # 클라이언트(조회 전용) 모드

# 명령어 파일을 읽어 실행 (-f 옵션)
./admin_shell -f commands.txt students.csv
```

CSV 파일 경로는 생략하면 기본값 `students.csv`를 사용한다.

## 명령어

| 명령어 | Admin | Client | 설명 |
|--------|:-----:|:------:|------|
| `list` | O | O | 전체 학생 목록 출력 |
| `find <id>` | O | O | 학번으로 학생 검색 |
| `stats` | O | O | 인원수·평균·최고·최저 점수 |
| `reload` | O | O | CSV에서 다시 불러오기 |
| `help` | O | O | 명령어 도움말 |
| `clear` | O | O | 화면 지우기 |
| `exit` | O | O | 프로그램 종료 |
| `add <id> <name> <score>` | O | X | 학생 추가 |
| `update <id> <score>` | O | X | 점수 수정 |
| `delete <id>` | O | X | 학생 삭제 |
| `sort <name\|score>` | O | X | 이름 또는 점수로 정렬 |
| `save` | O | X | CSV에 저장 |

클라이언트에서 관리자 전용 명령어를 입력하면 권한 거부 메시지가 출력된다.

## CSV 형식

```
id,name,score
1,Alice,90
2,Bob,85
3,Caleb,95
```

- `id`: 1 이상의 정수, 중복 불가
- `name`: 쉼표 없이, 최대 31자
- `score`: 0~100 정수

첫 줄 헤더(`id,name,score`)가 맞지 않으면 읽기를 중단하고, 중간에 형식이 잘못된 줄은 건너뛴 뒤 정상 데이터만 불러온다.

## 명령어 파일 형식

- 한 줄에 명령어 하나
- `#`으로 시작하는 줄과 빈 줄은 무시
- 실행 시 `[command file:N]` 형태로 줄 번호를 함께 출력하고, 오류가 난 줄은 건너뛴 뒤 다음 명령어를 계속 처리한다

```
# 예시 commands.txt
list
add 4 David 88
update 99 70
find 4
save
exit
```

## 파일 구조

```
.
├── main.c        # 진입점, 셸 루프, 명령행 인자 처리
├── student.h/.c  # Student 구조체, 연결 리스트 조작
├── file_io.h/.c  # CSV 불러오기/저장
├── command.h/.c  # 명령어 테이블, 입력 파싱, 예외 처리
├── Makefile
├── students.csv
└── README.md
```
