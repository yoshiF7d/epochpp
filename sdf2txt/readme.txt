sdf2txtビルド方法：./build

sdf2txt
	.sdfファイルをテキストデータにする。
	使い方 : sdf2txt -h

sdfs2txts　<- 本命
	フォルダに入った.sdfファイルを一度にテキスト形式にする。
	使い方：sdfs2txts [フォルダ名] [id名] [ダンプファイル間のタイムステップ]

sdfid　<-　上のプログラムの補助
	一つの.sdfファイルを入力としてid名の一覧（sdfs2txtsの入力に使う）を表示する。
	使い方 sdfid [.sdfファイル]