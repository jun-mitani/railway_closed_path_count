プラレールの 直線・1/4曲線レールの組み合わせで作ることができるコースの数をカウントするプログラムです。  
次のようなルールでカウントします。  
・閉じたコースとなる（始点と終点で位置と向きが一致する）  
・コースの交差、コースの一部の重なり合いを許容する  
・回転して一致するもの、または、反転して一致するものは同じものとみなして二重カウントしない。  
  
このプログラムを実行して得られた、使用するレールの数と作れるコースの数の対応は次の通りです。  
  
レールの数	作れるコースの数  
18	161  
20	847  
22	4,739  
24	29,983  
26	198,683  
28	1,375,928  
30	9,786,630  
  
Visual Studio 10 でコンパイル、実行しています。  
文字コードはSJISになっています。  

