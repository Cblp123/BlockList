#include <stdio.h>
#include <stdlib.h>
#define LEN_MAS 2
#define LEN_DOP_MAS 2
#define LEN_STR 20
#define M 3 //размер блока
#define L 5 //размер индексной таблицы

// Структура с данными узла
typedef struct Data {
    int mas[LEN_MAS];
    char str[LEN_STR];
    int dop_mas[LEN_DOP_MAS];
} Data;

// Структура блока
typedef struct BlockNode {
    Data data[M];
    int count;
    struct BlockNode* Next;
    struct BlockNode* Prev;
} BlockNode;

//  Структура индексной ячейки
typedef struct IndexRow {
    int NumberBlock;
    int Index;
    BlockNode* Elem;
} IndexRow;

// Структура индексированного блокового списка
typedef struct BlockList {
    BlockNode* Head;
    BlockNode* Tail;
    IndexRow IndexTable[L];
    int count;
    int step; //сохранение прошлого шага индексной таблицы
} BlockList;

// Создание нового пустого блока
BlockNode* New_BlockNode() {
    BlockNode* new_BlockNode = (BlockNode*)malloc(sizeof(BlockNode));
    new_BlockNode->count = 0;
    new_BlockNode->Prev = NULL;
    new_BlockNode->Next = NULL;
    return new_BlockNode;
}

// Инициализация нового пустого блокового списка
BlockList* new_BlockList() {
    BlockList* blockList = (BlockList*)malloc(sizeof(BlockList));
    blockList->Head = NULL;
    blockList->Tail = NULL;
    blockList->count = 0;
    blockList->IndexTable[0].Elem = NULL;
    blockList->step = 1;
    return blockList;
}

//Очистка блокового списка
void free_BlockList(BlockList* blockList) {
    BlockNode* current_node = blockList->Head;
    while (current_node != NULL) {
        BlockNode* next_node = current_node->Next;
        free(current_node);
        current_node = next_node;
    }
    free(blockList);
}
// Перестройка индексной таблицы
void rebuild_IndexTable(BlockList* blockList, int flag_del) {

    int countBlock = blockList->count;
    float newStep = countBlock * 1.0 / (L - 1);

    // проверка на необходимость перестройки с помощью сравнения с прошлым шагом
    if ((blockList->step == (int)newStep) && (flag_del != 1)) { //если было удаление - однозначно перестраиваем табицу
        return;
    }

    BlockNode* currentBlock = blockList->Head;
    int NumberTable = 1; // 0-ой элемент в индексной таблице всегда 0-ой блок
    int numberBlock = 0, currentIndex = 0;
    float next = newStep;
    float prev = 0;
    int x = (int)(next)-(int)prev; //учитываем дробность шага

    while (numberBlock < countBlock) { //основная перестройка
        if (numberBlock == x) {

            blockList->IndexTable[NumberTable].Elem = currentBlock;
            blockList->IndexTable[NumberTable].NumberBlock = numberBlock;
            blockList->IndexTable[NumberTable].Index = currentIndex;
            next += newStep;
            prev += newStep;
            x += (int)next - (int)prev;
            NumberTable++;
        }

        if (currentBlock->Next != NULL) {
            currentIndex += currentBlock->count;
            currentBlock = currentBlock->Next;
        }
        else {
            break;
        }
        numberBlock++;
        next += newStep;
        prev += newStep;
    }

    // Обязательно последний элемент в таблице - это последний блок
    while (1) {
        if (currentBlock->Next != NULL) {
            numberBlock++;
            currentIndex += currentBlock->count;
            currentBlock = currentBlock->Next;
        }
        else {
            break;
        }
    }
    blockList->IndexTable[L - 1].Elem = currentBlock;
    blockList->IndexTable[L - 1].NumberBlock = numberBlock;
    blockList->IndexTable[L - 1].Index = currentIndex;

    //обновляем шаг
    blockList->step = (int)newStep;
}

// Вывод одного элемента типа Data
void printData(Data data) {
    printf("%d %d %s %d %d\n", data.mas[0], data.mas[1], data.str, data.dop_mas[0], data.dop_mas[1]);
}

//Поиск по индексу
void FindElement(BlockList* blockList, int position) {
    for (int i = 0; i < L; i++) {
        if (blockList->IndexTable[i].Elem == NULL) {
            break;
        }
        if (blockList->IndexTable[i].Index == position) {
            printData(blockList->IndexTable[i].Elem->data[0]);
            return;
        }
        if (blockList->IndexTable[i].Index > position) {
            int index = blockList->IndexTable[i - 1].Index;
            BlockNode* currentBlock = blockList->IndexTable[i - 1].Elem;
            while (1) {

                if ((position - index) < currentBlock->count) { // элемент находится в текузем блоке
                    printData(currentBlock->data[position - index]);
                    return;
                }
                else { //иначе переходим к следующему блоку
                    index += currentBlock->count;
                    currentBlock = currentBlock->Next;
                }
            }

        }                   // Проверяем, может ли элемент находиться в последнем блоке
        if ((i == L - 1) && (blockList->IndexTable[i].Elem->count > (position - blockList->IndexTable[i].Index))) {
            printData(blockList->IndexTable[i].Elem->data[position - blockList->IndexTable[i].Index]);
            return;
        }
    }
    printf("Элемент не найден :(\n");
}

//Редактирование элемента 
void EditElement(BlockList* blockList, Data data, int position) {
    for (int i = 1; i < L; i++) {
        if (blockList->IndexTable[i].Elem == NULL) {
            break;
        }
        if (blockList->IndexTable[i].Index == position) {
            blockList->IndexTable[i].Elem->data[0] = data;
            return;
        }
        if (blockList->IndexTable[i].Index > position) {
            int index = blockList->IndexTable[i - 1].Index;
            BlockNode* currentBlock = blockList->IndexTable[i - 1].Elem;
            while (1) {
                if ((position - index) < currentBlock->count) {
                    currentBlock->data[position - index] = data;
                    return;

                }
                else {
                    index += currentBlock->count;
                    currentBlock = currentBlock->Next;
                }
            }

        }
        if ((i == L - 1) && (blockList->IndexTable[i].Elem->count > (position - blockList->IndexTable[i].Index))) {
            blockList->IndexTable[i].Elem->data[position - blockList->IndexTable[i].Index] = data;
            return;
        }
    }
    printf("Элемент не найден :(\n");
}

//Вывод информации 
void printList(BlockList* blockList) {
    BlockNode* currentBlock = blockList->Head;
    int count = 0;
    Data data;
    printf("\nВывод информации\n");
    while (currentBlock != NULL) {
        printf("\n\t%d block \n\n", count);
        for (int i = 0; i < currentBlock->count; i++) {
            data = currentBlock->data[i];
            printf("%d: ", i);
            printData(data);
        }
        currentBlock = currentBlock->Next;
        count += 1;
    }
    printf("\n\tIndex Table\n\n");
    for (int i = 0; i < L; i++) {
        if (blockList->IndexTable[i].Elem == NULL) {
            break;
        }
        data = blockList->IndexTable[i].Elem->data[0];
        printf("%d block, %d index: %d %d %s %d %d\n", blockList->IndexTable[i].NumberBlock, blockList->IndexTable[i].Index, data.mas[0], data.mas[1], data.str, data.dop_mas[0], data.dop_mas[1]);
    }
    printf("\n");
}

//Вставка в начало
void InsertToHead(BlockList* blockList, Data data) {
    if (blockList->Head == NULL) { //проверка на пустоту
        blockList->count = 1;
        BlockNode* new_blockNode = New_BlockNode();
        new_blockNode->data[0] = data;
        new_blockNode->count = 1;
        blockList->Head = blockList->Tail = new_blockNode;
        blockList->IndexTable[0].NumberBlock = 0;
        blockList->IndexTable[0].Elem = new_blockNode;
        blockList->IndexTable[0].Index = 0;
        blockList->IndexTable[0].NumberBlock = 0;

    }
    else if (blockList->Head->count < M) { // если хватает места для нового элемента
        int count = blockList->Head->count;
        for (int i = count; i > 0; i--) {
            blockList->Head->data[i] = blockList->Head->data[i - 1];
        }
        blockList->Head->data[0] = data;

        blockList->IndexTable[0].Elem = blockList->Head;
        blockList->Head->count += 1;
        for (int i = 1; i < L; i++) {
            if (blockList->IndexTable[i].Elem == NULL) {
                break;
            }
            blockList->IndexTable[i].Index += 1; // при добавлении элемента просто увеличиваем в таблице все индексы на 1
        }
    }
    else { // иначе создаем новый блок
        blockList->count += 1;
        BlockNode* new_blockNode = New_BlockNode();
        new_blockNode->count = 1;
        new_blockNode->data[0] = blockList->Head->data[M - 1];
        if (blockList->Head->Next != NULL) {
            blockList->Head->Next->Prev = new_blockNode;
            new_blockNode->Next = blockList->Head->Next;
            blockList->Head->Next = new_blockNode;
        }
        else {
            blockList->Head->Next = new_blockNode;
            blockList->Tail = new_blockNode;
            new_blockNode->Prev = blockList->Head;
        }



        for (int i = M - 1; i > 0; i--) {
            blockList->Head->data[i] = blockList->Head->data[i - 1];

        }
        blockList->Head->data[0] = data;
        blockList->IndexTable[0].Elem = blockList->Head;
        if (blockList->count <= L) {
            for (int i = blockList->count - 1; i > 1; i--) {
                blockList->IndexTable[i].Index = blockList->IndexTable[i - 1].Index + 1;
                blockList->IndexTable[i].Elem = blockList->IndexTable[i - 1].Elem;
                blockList->IndexTable[i].NumberBlock = blockList->IndexTable[i - 1].NumberBlock + 1;


            }
            blockList->IndexTable[1].Elem = blockList->Head->Next;
            blockList->IndexTable[1].Index = blockList->Head->count;
            blockList->IndexTable[1].NumberBlock = 1;


        }
        else {
            for (int i = 1; i < L; i++) {
                blockList->IndexTable[i].Index++;
                blockList->IndexTable[i].NumberBlock++;
            }
            printf("Нужна равномерная перестройка индексной таблицы\n");
            rebuild_IndexTable(blockList, 0);
        }


    }

}

//Вставка в конец
void InsertToTail(BlockList* blockList, Data data) {
    if (blockList->Tail == NULL) {
        blockList->count = 1;
        BlockNode* new_blockNode = New_BlockNode();
        new_blockNode->data[0] = data;
        new_blockNode->count = 1;
        blockList->Tail = blockList->Head = new_blockNode;
        blockList->IndexTable[0].NumberBlock = 0;
        blockList->IndexTable[0].Elem = new_blockNode;
        blockList->IndexTable[0].Index = 0;
        blockList->IndexTable[0].NumberBlock = 0;

    }
    else if (blockList->Tail->count < M) {
        blockList->Tail->data[blockList->Tail->count] = data;
        blockList->Tail->count += 1;

    }
    else {
        blockList->count += 1;
        BlockNode* new_blockNode = New_BlockNode();
        new_blockNode->count = 1;
        new_blockNode->data[0] = data;

        new_blockNode->Prev = blockList->Tail;
        blockList->Tail->Next = new_blockNode;
        blockList->Tail = new_blockNode;

        if (blockList->count <= L) {
            blockList->IndexTable[blockList->count - 1].Elem = new_blockNode;
            blockList->IndexTable[blockList->count - 1].Index = blockList->IndexTable[blockList->count - 2].Elem->count + blockList->IndexTable[blockList->count - 2].Index;
            blockList->IndexTable[blockList->count - 1].NumberBlock = blockList->count - 1;

        }
        else {
            printf("Нужна равномерная перестройка индексной таблицы\n");
            rebuild_IndexTable(blockList, 0);
        }
    }
}

//Вставка на позицию
void Insert(BlockList* blockList, Data data, int position) {
    if ((position == 0) || (blockList->Head == NULL)) {
        InsertToTail(blockList, data);
        return;
    }
    int InsertPosition = position;
    int PositionInTable = 0;
    int L1 = L;
    if (L > blockList->count) {
        L1 = blockList->count;
    }
    BlockNode* currentBlock;
    for (int i = 0; i < L1; i++) {
        PositionInTable = i;

        if (blockList->IndexTable[PositionInTable].Index == InsertPosition) {
            currentBlock = blockList->IndexTable[PositionInTable].Elem;
            InsertPosition = 0;
            break;
        }
        if (blockList->IndexTable[PositionInTable].Index > position) {
            int index = blockList->IndexTable[PositionInTable - 1].Index;
            currentBlock = blockList->IndexTable[PositionInTable - 1].Elem;
            while (1) {
                if ((position - index) < currentBlock->count) {
                    break;
                }
                else {
                    index += currentBlock->count;
                    currentBlock = currentBlock->Next;
                }
            }
            InsertPosition = position - index;
            PositionInTable = i - 1;
            break;
        }
        if ((i == L1 - 1) && (M > (position - blockList->IndexTable[PositionInTable].Index))) {
            if (blockList->IndexTable[PositionInTable].Elem->count <= (position - blockList->IndexTable[PositionInTable].Index)) {
                InsertPosition = (position - blockList->IndexTable[i].Index) - (M - blockList->IndexTable[i].Elem->count - 1);
            }
            else {
                InsertPosition = (position - blockList->IndexTable[i].Index);
            }
            PositionInTable = i;
            currentBlock = blockList->IndexTable[PositionInTable].Elem;
            break;
        }
        if ((i == L1 - 1) && (M <= (position - blockList->IndexTable[i].Index))) {
            InsertToTail(blockList, data);
            return;
        }

    }
    int count = currentBlock->count;
    if (count < M) {
        for (int i = count; i > InsertPosition; i--) {
            currentBlock->data[i] = currentBlock->data[i - 1];
        }
        currentBlock->data[InsertPosition] = data;
        currentBlock->count++;
        for (int i = PositionInTable + 1; i < L; i++) {
            blockList->IndexTable[i].Index++;
        }
    }
    else {
        blockList->count += 1;
        BlockNode* new_blockNode = New_BlockNode();
        new_blockNode->count = 1;
        new_blockNode->data[0] = currentBlock->data[M - 1];
        if (currentBlock->Next != NULL) {
            currentBlock->Next->Prev = new_blockNode;
            new_blockNode->Next = currentBlock->Next;
            currentBlock->Next = new_blockNode;
        }
        else {
            currentBlock->Next = new_blockNode;
            blockList->Tail = new_blockNode;
            new_blockNode->Prev = currentBlock;
        }


        for (int i = M - 1; i > InsertPosition; i--) {
            currentBlock->data[i] = currentBlock->data[i - 1];
        }

        currentBlock->data[InsertPosition] = data;
        if (blockList->count < L) {
            for (int i = blockList->count; i > PositionInTable + 1; i--) {
                blockList->IndexTable[i].Index = blockList->IndexTable[i - 1].Index + 1;
                blockList->IndexTable[i].Elem = blockList->IndexTable[i - 1].Elem;
                blockList->IndexTable[i].NumberBlock = blockList->IndexTable[i - 1].NumberBlock + 1;
            }
            blockList->IndexTable[PositionInTable + 1].Index = blockList->IndexTable[PositionInTable].Index + M;
            blockList->IndexTable[PositionInTable + 1].Elem = currentBlock;
            blockList->IndexTable[PositionInTable + 1].NumberBlock = blockList->IndexTable[PositionInTable].NumberBlock + 1;
        }
        else {
            for (int i = PositionInTable + 1; i < L; i++) {
                blockList->IndexTable[i].Index++;
                blockList->IndexTable[i].NumberBlock++;
            }
            printf("Нужна равномерная перестройка индексной таблицы\n");
            rebuild_IndexTable(blockList, 0);
        }
    }
}

//Удаление по индексу
void Delete(BlockList* blockList, int position) {
    if (blockList->Head == NULL) {
        printf("Список пуст\n");
        return;
    }
    int DeletePosition = position;
    int PositionInTable = 0;
    int L1 = L;
    if (L > blockList->count) {
        L1 = blockList->count;
    }
    BlockNode* currentBlock = New_BlockNode();
    int flag = 0;
    for (int i = 0; i < L1; i++) {
        PositionInTable = i;
        if (blockList->IndexTable[PositionInTable].Index == DeletePosition) {
            currentBlock = blockList->IndexTable[PositionInTable].Elem;
            DeletePosition = 0;
            flag = 1;
            break;
        }
        if (blockList->IndexTable[PositionInTable].Index > position) {
            int index = blockList->IndexTable[PositionInTable - 1].Index;
            currentBlock = blockList->IndexTable[PositionInTable - 1].Elem;
            while (1) {
                if ((position - index) < currentBlock->count) {
                    break;
                }
                else {
                    index += currentBlock->count;
                    currentBlock = currentBlock->Next;
                }
            }
            DeletePosition = position - index;
            PositionInTable = i - 1;
            flag = 1;
            break;
        }
        if ((i == L1 - 1) && (currentBlock->count > (blockList->IndexTable[PositionInTable].Elem->count - blockList->IndexTable[PositionInTable].Index))) {
            DeletePosition = (position - blockList->IndexTable[i].Index);
            PositionInTable = i;
            currentBlock = blockList->IndexTable[PositionInTable].Elem;
            flag = 1;
            break;
        }
    }
    if (flag == 0) {
        printf("Такого элемента не существует\n");
        return;
    }

    int count = currentBlock->count;
    if (count > 1) {
        for (int i = DeletePosition; i < count - 1; i++) {
            currentBlock->data[i] = currentBlock->data[i + 1];
        }
        currentBlock->count--;
        for (int i = PositionInTable + 1; i < L; i++) {
            blockList->IndexTable[i].Index--;
        }
    }
    else {
        blockList->count--;
        if (currentBlock->Prev == NULL) {
            if (currentBlock->Next == NULL) {
                free_BlockList(blockList);
                BlockList* blockList = new_BlockList();
                return;
            }
            else {
                blockList->Head = currentBlock->Next;
                blockList->Head->Prev = NULL;
            }
        }
        else if (currentBlock->Next == NULL) {
            blockList->Tail = currentBlock->Prev;
            blockList->Tail->Next = NULL;
        }
        else {
            currentBlock->Next->Prev = currentBlock->Prev;
            currentBlock->Prev->Next = currentBlock->Next;
        }

        if (blockList->count < L) {
            for (int i = PositionInTable; i < blockList->count; i++) {
                blockList->IndexTable[i].Index = blockList->IndexTable[i + 1].Index - 1;
                blockList->IndexTable[i].Elem = blockList->IndexTable[i + 1].Elem;
                blockList->IndexTable[i].NumberBlock = blockList->IndexTable[i + 1].NumberBlock - 1;
            }
        }
        else {
            for (int i = PositionInTable; i < L - 1; i++) {
                blockList->IndexTable[i].Index = blockList->IndexTable[i + 1].Index - 1;
                blockList->IndexTable[i].Elem = blockList->IndexTable[i + 1].Elem;
                blockList->IndexTable[i].NumberBlock = blockList->IndexTable[i + 1].NumberBlock - 1;
            }
            printf("Нужна равномерная перестройка индексной таблицы\n");
            rebuild_IndexTable(blockList, 1);
        }
    }
}


int main() {
    int position;
    Data data;
    BlockList* blockList = new_BlockList();
    int function;
    while (1) {
        printf("0-добавНач, 1-добавКон, 2-Поиск, 3-Редакт, 4-Вставка, 5-Удал, 6-ПерестрИндТабл, 7-Вывод\n");
        printf("Выберите функцию: ");
        scanf("%d", &function);
        switch (function) {
        case 0:
            printf("Добавление элемента в начало\n");
            printf("Введите данные: ");
            scanf("%d %d %s %d %d", &data.mas[0], &data.mas[1], data.str, &data.dop_mas[0], &data.dop_mas[1]);
            InsertToHead(blockList, data);
            break;
        case 1:
            printf("Добавление элемента в конец\n");
            printf("Введите данные: ");
            scanf("%d %d %s %d %d", &data.mas[0], &data.mas[1], data.str, &data.dop_mas[0], &data.dop_mas[1]);
            InsertToTail(blockList, data);
            break;
        case 2:
            printf("Поиск по индексу\n");
            printf("Введите позицию элемента: ");
            scanf("%d", &position);
            FindElement(blockList, position);
            break;
        case 3:
            printf("Редактирование по индексу\n");
            printf("Введите позицию элемента: ");
            scanf("%d", &position);
            printf("Введите новые данные для этого элемента: ");
            scanf("%d %d %s %d %d", &data.mas[0], &data.mas[1], data.str, &data.dop_mas[0], &data.dop_mas[1]);
            EditElement(blockList, data, position);
            break;
        case 4:
            printf("Вставка по индексу \n");
            printf("Введите позицию вставки элемента: ");
            scanf("%d", &position);
            printf("Введите данные нового элемента: ");
            scanf("%d %d %s %d %d", &data.mas[0], &data.mas[1], data.str, &data.dop_mas[0], &data.dop_mas[1]);
            Insert(blockList, data, position);
            break;
        case 5:
            printf("Удаление по индексу\n");
            printf("Введите позицию удаляемого элемента: ");
            scanf("%d", &position);
            Delete(blockList, position);
            break;
        case 6:
            printf("Перестройка индексной талицы\n");
            rebuild_IndexTable(blockList, 0);
            break;
        case 7:
            printList(blockList);
            break;
        }
        printf("\n");
        if (function == -1) {
            break;
        }
    }
    free_BlockList(blockList);
    return 0;
}
