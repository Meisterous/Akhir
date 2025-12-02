#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/stat.h>

using namespace std;

struct MenuItem {
    string nama;
    int stok;
    int modal;
    int harga;
};

struct Transaksi {
    string namaMenu;
    int qty;
    int modalTotal;
    int pendapatan;
    int laba;
};

struct Pengeluaran {
    string ket;
    int nominal;
};

vector<MenuItem> menuList;
vector<Transaksi> trxList;
vector<Pengeluaran> pengList;

string encrypt(string text) {
    string hasil = "";
    for (char c : text) {
        hasil += (char)(c + 3);
    }
    return hasil;
}

string decrypt(string text) {
    string hasil = "";
    for (char c : text) {
        hasil += (char)(c - 3);
    }
    return hasil;
}

bool fileExists(string name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void ensureUserFile() {
    if (!fileExists("user.txt")) {
        ofstream f("user.txt");
        
        f << encrypt("admin") << " " << encrypt("admin123") << " " << encrypt("admin") << "\n";
        f << encrypt("kasir") << " " << encrypt("kasir123") << " " << encrypt("kasir") << "\n";
        
        f.close();
    
        cout << "\n[INFO] File user.txt tidak ditemukan -> dibuat otomatis.\n";
    }
}

void loadMenu() {
    menuList.clear();
    ifstream f("stok.txt");
    MenuItem m;
    while (f >> m.nama >> m.stok >> m.modal >> m.harga) {
        menuList.push_back(m);
    }
}

void saveMenu() {
    ofstream f("stok.txt");
    for (auto &m : menuList) {
        f << m.nama << " " << m.stok << " " << m.modal << " " << m.harga << "\n";
    }
}

void loadTransaksi() {
    trxList.clear();
    ifstream f("transaksi.txt");
    Transaksi t;
    while (f >> t.namaMenu >> t.qty >> t.modalTotal >> t.pendapatan >> t.laba) {
        trxList.push_back(t);
    }
}

void saveTransaksi() {
    ofstream f("transaksi.txt");
    for (auto &t : trxList) {
        f << t.namaMenu << " " << t.qty << " " << t.modalTotal << " " 
          << t.pendapatan << " " << t.laba << "\n";
    }
}

void loadPengeluaran() {
    pengList.clear();
    ifstream f("pengeluaran.txt");
    Pengeluaran p;
    while (f >> p.ket >> p.nominal) {
        pengList.push_back(p);
    }
}

void savePengeluaran() {
    ofstream f("pengeluaran.txt");
    for (auto &p : pengList) {
        f << p.ket << " " << p.nominal << "\n";
    }
}

bool login(string role) {
    ensureUserFile();

    ifstream f("user.txt");
    if (!f.is_open()) {
        cout << "Gagal membuka user.txt\n";
        return false;
    }

    string u_enc, p_enc, r_enc;
    string inputUser, inputPass;

    cout << "Username: ";
    cin >> inputUser;
    cout << "Password: ";
    cin >> inputPass;


    while (f >> u_enc >> p_enc >> r_enc) {
        string realUser = decrypt(u_enc);
        string realPass = decrypt(p_enc);
        string realRole = decrypt(r_enc);

        if (realUser == inputUser && realPass == inputPass && realRole == role) {
            return true;
        }
    }

    cout << "\n[LOGIN GAGAL] Username atau password salah.\n";
    return false;
}

void adminTambahMenu() {
    MenuItem m;
    cout << "Nama menu (tanpa spasi): "; cin >> m.nama;
    cout << "Stok awal: "; cin >> m.stok;
    cout << "Harga modal: "; cin >> m.modal;
    cout << "Harga jual: "; cin >> m.harga;

    menuList.push_back(m);
    saveMenu();
    cout << "Menu berhasil ditambahkan!\n";
}

void adminLaporan() {
    int totalModal = 0, totalPendapatan = 0, totalLabaKotor = 0, totalPengeluaran = 0;

    for (auto &t : trxList) {
        totalModal += t.modalTotal;
        totalPendapatan += t.pendapatan;
        totalLabaKotor += t.laba;
    }
    for (auto &p : pengList) {
        totalPengeluaran += p.nominal;
    }

    cout << "\n======== LAPORAN WARTEG ========\n";
    cout << "Total Modal         : " << totalModal << endl;
    cout << "Total Pendapatan    : " << totalPendapatan << endl;
    cout << "Laba Kotor          : " << totalLabaKotor << endl;
    cout << "Pengeluaran         : " << totalPengeluaran << endl;
    cout << "Laba Bersih         : " << totalLabaKotor - totalPengeluaran << endl;
}

void adminMenu() {
    int pilih;
    while (true) {
        cout << "\n===== MENU ADMIN =====\n";
        cout << "1. Tambah Menu\n";
        cout << "2. Lihat Laporan Keuangan\n";
        cout << "3. Kembali\n";
        cout << "Pilih: ";
        cin >> pilih;

        if (pilih == 1) adminTambahMenu();
        else if (pilih == 2) adminLaporan();
        else if (pilih == 3) break;
        else cout << "Salah input!\n";
    }
}

void kasirTransaksi() {
    string nama;
    int qty;

    cout << "Nama menu: "; cin >> nama;
    cout << "Jumlah: "; cin >> qty;

    for (auto &m : menuList) {
        if (m.nama == nama) {
            if (m.stok < qty) {
                cout << "Stok tidak cukup!\n";
                return;
            }

            m.stok -= qty;

            Transaksi t;
            t.namaMenu = nama;
            t.qty = qty;
            t.modalTotal = qty * m.modal;
            t.pendapatan = qty * m.harga;
            t.laba = t.pendapatan - t.modalTotal;

            trxList.push_back(t);
            saveMenu();
            saveTransaksi();

            cout << "Transaksi berhasil!\n";
            return;
        }
    }
    cout << "Menu tidak ditemukan!\n";
}

void kasirMenu() {
    int pilih;
    while (true) {
        cout << "\n===== MENU KASIR =====\n";
        cout << "1. Transaksi Pelanggan\n";
        cout << "2. Kembali\n";
        cout << "Pilih: "; cin >> pilih;

        if (pilih == 1) kasirTransaksi();
        else if (pilih == 2) break;
        else cout << "Input salah!\n";
    }
}

int main() {
    loadMenu();
    loadTransaksi();
    loadPengeluaran();

    int pilih;
    while (true) {
        cout << "\n===== LOGIN SYSTEM (SECURE) =====\n";
        cout << "1. Login Admin\n";
        cout << "2. Login Kasir\n";
        cout << "3. Keluar\n";
        cout << "Pilih: ";
        cin >> pilih;

        if (pilih == 1) {
            if (login("admin")) adminMenu();
        }
        else if (pilih == 2) {
            if (login("kasir")) kasirMenu();
        }
        else if (pilih == 3) break;
        else cout << "Input salah!\n";
    }

    return 0;
}