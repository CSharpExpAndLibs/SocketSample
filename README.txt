rsyslogをリモートログサーバーに設定する

1. /etc/rsyslog.confを編集してTCP経由の入力を受け付ける設定にする
module(load="imtcp")
input(type="imtcp" port="514")

2. DomainSockServerに対してloggerで擬似ログを送付してファシリティを指
   示する数値を調べる。例えば、
     logger -p local0.alert -u /tmp/domaind.socket 'POOOOOOO'
   と入力すると、
     <129>Jul  8 17:24:59 charan: POOOOOOO
   と出力されるので、'local0.alert'は129であることが分かる。

3. Nlogクライアントをどのファシリティに当てはめるか決めてそのファシリ
   ティを指示する数値NをNLOGメッセージに'<N>'で埋め込む。例え
   ば'local0.alert'であれば<129>という文字列を先頭に埋め込むことになる。

4. /etc/rsyslog.d/yy-xxx.confを作成してNLOGが発行したログメッセージの
   格納の仕方を記述する。



