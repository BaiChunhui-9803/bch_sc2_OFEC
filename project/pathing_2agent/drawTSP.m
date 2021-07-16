%%
%最优解示意图
clc;
clear;
close all;

BestSolution=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/best_solution.txt');
UnitSize=size(BestSolution,1);

figure(1)
set(gcf,'position',[3,245,728,589])

subplot(2,2,1);
set(gca,'position', [0.08 0.58 0.38 0.38]);
for i=1:UnitSize/2-1
    plot([BestSolution(i,1),BestSolution(i+1,1)],[BestSolution(i,2),BestSolution(i+1,2)],'ro-','LineWidth',1,'MarkerEdgeColor','k','MarkerFaceColor','r');  
    text(BestSolution(i,1),BestSolution(i,2),[' ',int2str(i-1)]);  
    text(BestSolution(i+1,1),BestSolution(i+1,2),[' ',int2str(i)]);  
    hold on;  
end

for i=UnitSize:-1:UnitSize/2+2
    plot([BestSolution(i,1),BestSolution(i-1,1)],[BestSolution(i,2),BestSolution(i-1,2)],'bo-','LineWidth',1,'MarkerEdgeColor','k','MarkerFaceColor','b');  
    text(BestSolution(i,1),BestSolution(i,2),[' ',int2str(i-1)]);  
    text(BestSolution(i-1,1),BestSolution(i-1,2),[' ',int2str(i-2)]);  
    hold on;  
end

title('最优解示意图'); 
xlabel('\it\fontname{Times New Roman}X axis','FontName','Times New Roman','FontSize',12);
ylabel('\it\fontname{Times New Roman}Y axis','FontName','Times New Roman','FontSize',12);
%%
%全局最优路径
subplot(2,2,2);
set(gca,'position', [0.58 0.58 0.38 0.38]);
gBest=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/dist_gbest.txt');
x=gBest(:,1);
y=gBest(:,2);
sz=100;
mkr='.';
c=linspace(1,20000,length(x));
scatter(x,y,sz,c,mkr)
title('全局最优路径')
xlabel('迭代次数\it\fontname{Times New Roman}(Number of iteration)','FontSize',12);
ylabel('路径长度\it\fontname{Times New Roman}(path length)','FontSize',12);
%%
%种群最优路径
subplot(2,2,3);
set(gca,'position', [0.08 0.08 0.38 0.38]);
popBest=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/pop_best.txt');
x=popBest(:,1);
y=popBest(:,2);
sz=100;
mkr='.';
c=linspace(1,20000,length(x));
scatter(x,y,sz,c,mkr)
title('种群最优路径')
xlabel('迭代次数\it\fontname{Times New Roman}(Number of iteration)','FontSize',12);
ylabel('路径长度\it\fontname{Times New Roman}(path length)','FontSize',12);
%%
%种群平均路径
subplot(2,2,4);
set(gca,'position', [0.58 0.08 0.38 0.38]);
popAvg=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/pop_avg.txt');
x=popAvg(:,1);
y=popAvg(:,2);
sz=100;
mkr='.';
c=linspace(1,20000,length(x));
scatter(x,y,sz,c,mkr)
title('种群平均路径')
xlabel('迭代次数\it\fontname{Times New Roman}(Number of iteration)','FontSize',12);
ylabel('路径长度\it\fontname{Times New Roman}(path length)','FontSize',12);
%%
%不同算法效果对比

figure(2)
set(gcf,'position',[740,570,525,420])

comP2A=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_1.txt');
comP1A=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_bot.txt');
comN2=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_nearest_2agent.txt');
comN1=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_nearest_bot.txt');
x1=comP2A(:,1);
y1=comP2A(:,2);
x2=comP1A(:,1);
y2=comP1A(:,2);
x3=comN2(:,1);
y3=comN2(:,2);
x4=comN1(:,1);
y4=comN1(:,2);
sz=1;
mkr='.';
c1='r';
c2='m';
c3='b';
c4='g';
scatter(x1,y1,sz,c1,mkr)
hold on
scatter(x2,y2,sz,c2,mkr)
hold on
scatter(x3,y3,sz,c3,mkr)
hold on
scatter(x4,y4,sz,c4,mkr)
legend('GA\_2agent','GA\_1agent','Near\_2agent','Near\_1agent','Location','northwest')
xlim([0,30000]); % 设置坐标轴范围  
ylim([0,350000]);
title('不同算法效果对比')
xlabel('游戏帧\it\fontname{Times New Roman}(Game Loop)','FontSize',12);
ylabel('收集资源总数\it\fontname{Times New Roman}(Collected Minerals)','FontSize',12);
box on


%%
%GA_2Agent不同GA配置效果对比

figure(3)
set(gcf,'position',[740,60,525,420])

comP2A_1=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_1.txt');
comP2A_2=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_2.txt');
comP2A_3=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_3.txt');
comP2A_4=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_4.txt');
comP2A_5=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_5.txt');
comP2A_6=importdata('D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent_6.txt');
x1=comP2A_1(:,1);
y1=comP2A_1(:,2);
x2=comP2A_2(:,1);
y2=comP2A_2(:,2);
x3=comP2A_3(:,1);
y3=comP2A_3(:,2);
x4=comP2A_4(:,1);
y4=comP2A_4(:,2);
x5=comP2A_5(:,1);
y5=comP2A_5(:,2);
x6=comP2A_6(:,1);
y6=comP2A_6(:,2);
sz=1;
mkr='.';
c1='k';
c2='g';
c3='b';
c4='c';
c5='r';
c6='m';

scatter(x1,y1,sz,c1,mkr)
hold on
scatter(x2,y2,sz,c2,mkr)
hold on
scatter(x3,y3,sz,c3,mkr)
hold on
scatter(x4,y4,sz,c4,mkr)
hold on
scatter(x5,y5,sz,c5,mkr)
hold on
scatter(x6,y6,sz,c6,mkr)
box on
title('GA\_2Agent不同GA配置效果对比')
xlim([0,30000]); % 设置坐标轴范围  
ylim([0,350000]);
xlabel('游戏帧\it\fontname{Times New Roman}(Game Loop)','FontSize',12);
ylabel('收集资源总数\it\fontname{Times New Roman}(Collected Minerals)','FontSize',12);
legend('GA\_2agent\_1','GA\_2agent\_2','GA\_2agent\_3','GA\_2agent\_4','GA\_2agent\_5','GA\_2agent\_6','Location','northwest')

axes('Position',[0.54,0.2,0.32,0.20]); % 生成子图   左右  上下 宽窄
scatter(x1,y1,sz,c1,mkr)
hold on
scatter(x2,y2,sz,c2,mkr)
hold on
scatter(x3,y3,sz,c3,mkr)
hold on
scatter(x4,y4,sz,c4,mkr)
hold on
scatter(x5,y5,sz,c5,mkr)
hold on
scatter(x6,y6,sz,c6,mkr)
box on
xlim([27000,28700]); % 设置坐标轴范围  

