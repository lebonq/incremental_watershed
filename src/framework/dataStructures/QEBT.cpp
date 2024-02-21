//
// Created by lebon on 23/05/2022.
//

#include "QEBT.h"

QEBT::QEBT(int nbVertex) :
    qbt{nbVertex},
    qt{nbVertex},
    root_{new int[nbVertex*2 -1]}
{}

int QEBT::findCannonical(int q) {
    return this->qt.findCannonical(q);
}

void QEBT::makeSet(int q) {
    this->root_[q] = q;
    this->qbt.makeSet(q);
    this->qt.makeSet(q);
}

int QEBT::makeUnion(int q1, int q2) {
    int tu = this->root_[q1];
    int tv = this->root_[q2];
    //union for BT (without path compression)
    this->qbt.setParent(tu,this->qbt.getSize());
    this->qbt.setParent(tv,this->qbt.getSize());
    //union for QT (with path compression)
    int c = this->qt.makeUnion(q1,q2);
    this->root_[c] = this->qbt.getSize();
    this->qbt.makeSet(this->qbt.getSize());
    return this->qbt.getSize() - 1;

}

void QEBT::clean_memory()
{
    delete[] root_;
    this->qt.cleanMemory();
}

QEBT::~QEBT() {

}