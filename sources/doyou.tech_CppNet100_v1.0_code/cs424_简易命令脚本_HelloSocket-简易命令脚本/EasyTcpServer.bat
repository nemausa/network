::�����IP��ַ
@set strIP=any
::����˶˿�
@set nPort=4567
::��Ϣ�߳����� CellServer
@set nThread=1
::���ƿͻ������� δ����
@set nClient=3

EasyTcpServer %strIP% %nPort% %nThread% %nClient%

@pause