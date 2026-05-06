# BUILD_NOT_RUN_IN_CHATGPT

このChatGPT実行環境にはGBDKの lcc が無いため、実ビルドは未実行。

ユーザー環境で以下を確認すること。

```sh
./build.sh
```

特に romusage の以下を確認する。

- `_HOME` overflow が出ないこと
- `Write from one bank spans into the next` が出ないこと
- 戦闘UI位置が rpg088 から変化していないこと
