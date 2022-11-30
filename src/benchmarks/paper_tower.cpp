//
// Created by lebonq on 18/11/22.
//

#include "paper_tower.h"
#include "../framework/data.h"
#include "../markers.h"
#include "../framework/dataStructures/graph.h"
#include "../framework/imageManager.h"
#include "../framework/meyer_ws.h"
#include <opencv2/highgui/highgui.hpp>
#include <benchmark/benchmark.h>

static void BM_MEYER_STEP1(benchmark::State& state){

    for (auto _ : state) {
        state.PauseTiming();
        auto meyer = meyer_ws("holiday_data/tower.jpg");
        meyer.pre_process(0);
        state.ResumeTiming();
        meyer.ws();
        state.PauseTiming();
    }
}

static void BM_IW_STEP1(benchmark::State& state){
    auto testim = cv::imread("holiday_data/tower.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        imageManager testImg = imageManager("holiday_data/tower.jpg",testim);
        testImg.init();
        testImg.addMarkers(markerTower,nbmarkerTower,false);
    }
}

static void BM_MEYER_STEP2(benchmark::State& state){

    for (auto _ : state) {
        state.PauseTiming();
        auto meyer = meyer_ws("holiday_data/tower.jpg");
        meyer.pre_process(1);
        state.ResumeTiming();
        meyer.ws();
        state.PauseTiming();
    }
}

static void BM_IW_STEP2(benchmark::State& state){
    auto testim = cv::imread("holiday_data/tower.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("holiday_data/tower.jpg",testim);
        testImg.init();
        testImg.addMarkers(markerTower,nbmarkerTower,false);
        state.ResumeTiming();
        testImg.addMarkers(markerTower2,nbmarkerTower2,false);
    }
}

static void BM_IW_BATCH_STEP2(benchmark::State& state){
    auto testim = cv::imread("holiday_data/tower.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("holiday_data/tower.jpg",testim);
        testImg.init();
        state.ResumeTiming();
        testImg.addMarkers(markerTower_batch,nbmarkbatch,false);
    }
}

#define bench_num 100
// Register the function as a benchmark
BENCHMARK(BM_MEYER_STEP1)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_IW_STEP1)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_MEYER_STEP2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_IW_STEP2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_IW_BATCH_STEP2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK_MAIN();