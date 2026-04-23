import UIKit
import InputMethodKit

/**
 * iOS输入法扩展视图控制器
 * 支持iOS 12.0及更高版本
 */
class InputViewController: UIInputViewController {

    private var candidateView: UICollectionView!
    private var candidateData: [String] = []
    private var selectedIndex: Int = 0

    private var heightConstraint: NSLayoutConstraint!

    override func viewDidLoad() {
        super.viewDidLoad()

        setupCandidateView()
        setupInputButtons()
    }

    override func viewWillLayoutSubviews() {
        super.viewWillLayoutSubviews()
        updateCandidateViewFrame()
    }

    private func setupCandidateView() {
        let layout = UICollectionViewFlowLayout()
        layout.scrollDirection = .horizontal
        layout.minimumInteritemSpacing = 8
        layout.minimumLineSpacing = 8

        candidateView = UICollectionView(frame: .zero, collectionViewLayout: layout)
        candidateView.backgroundColor = UIColor.systemBackground
        candidateView.register(CandidateCell.self,
            forCellWithReuseIdentifier: "CandidateCell")
        candidateView.dataSource = self
        candidateView.delegate = self
        candidateView.translatesAutoresizingMaskIntoConstraints = false

        view.addSubview(candidateView)

        NSLayoutConstraint.activate([
            candidateView.topAnchor.constraint(equalTo: view.topAnchor),
            candidateView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            candidateView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            candidateView.heightAnchor.constraint(equalToConstant: 44)
        ])
    }

    private func setupInputButtons() {
        let stackView = UIStackView()
        stackView.axis = .horizontal
        stackView.distribution = .fillEqually
        stackView.spacing = 4
        stackView.translatesAutoresizingMaskIntoConstraints = false

        let nextKeyboardButton = createButton(title: "🌐", action: #selector(handleInputModeList))
        let spaceButton = createButton(title: "空格", action: #selector(insertSpace))
        let deleteButton = createButton(title: "⌫", action: #selector(deleteBackward))
        let returnButton = createButton(title: "换行", action: #selector(insertNewline))

        stackView.addArrangedSubview(nextKeyboardButton)
        stackView.addArrangedSubview(spaceButton)
        stackView.addArrangedSubview(deleteButton)
        stackView.addArrangedSubview(returnButton)

        view.addSubview(stackView)

        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: candidateView.bottomAnchor, constant: 4),
            stackView.leadingAnchor.constraint(equalTo: view.leadingAnchor, constant: 4),
            stackView.trailingAnchor.constraint(equalTo: view.trailingAnchor, constant: -4),
            stackView.heightAnchor.constraint(equalToConstant: 40)
        ])
    }

    private func createButton(title: String, action: Selector) -> UIButton {
        let button = UIButton(type: .system)
        button.setTitle(title, for: .normal)
        button.titleLabel?.font = UIFont.systemFont(ofSize: 16)
        button.backgroundColor = UIColor.secondarySystemBackground
        button.layer.cornerRadius = 4
        button.addTarget(self, action: action, for: .touchUpInside)
        return button
    }

    private func updateCandidateViewFrame() {
        guard let inputView = inputView else { return }
        heightConstraint?.constant = 216
    }

    func updateCandidates(_ candidates: [String], selected: Int) {
        candidateData = candidates
        selectedIndex = selected
        candidateView.reloadData()
    }

    func commitText(_ text: String) {
        textDocumentProxy.insertText(text)
    }

    @objc private func insertSpace() {
        textDocumentProxy.insertText(" ")
    }

    @objc private func deleteBackward() {
        textDocumentProxy.deleteBackward()
    }

    @objc private func insertNewline() {
        textDocumentProxy.insertText("\n")
    }

    override func textDidChange(_ textInput: UITextInput?) {
        guard let proxy = textDocumentProxy else { return }
        let text = proxy.documentContextBeforeInput ?? ""
        processInput(text)
    }

    private func processInput(_ text: String) {
        // 通过桥接层调用核心引擎处理输入
        ImeBridge.shared.processInput(text) { [weak self] candidates in
            DispatchQueue.main.async {
                self?.updateCandidates(candidates.map { $0.text }, selected: 0)
            }
        }
    }
}

// MARK: - UICollectionViewDataSource
extension InputViewController: UICollectionViewDataSource {
    func collectionView(_ collectionView: UICollectionView,
        numberOfItemsInSection section: Int) -> Int {
        return candidateData.count
    }

    func collectionView(_ collectionView: UICollectionView,
        cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(
            withReuseIdentifier: "CandidateCell", for: indexPath) as! CandidateCell
        cell.configure(text: candidateData[indexPath.item],
            isSelected: indexPath.item == selectedIndex)
        return cell
    }
}

// MARK: - UICollectionViewDelegate
extension InputViewController: UICollectionViewDelegate {
    func collectionView(_ collectionView: UICollectionView,
        didSelectItemAt indexPath: IndexPath) {
        if indexPath.item < candidateData.count {
            commitText(candidateData[indexPath.item])
            candidateData.removeAll()
            collectionView.reloadData()
        }
    }
}

// MARK: - UICollectionViewDelegateFlowLayout
extension InputViewController: UICollectionViewDelegateFlowLayout {
    func collectionView(_ collectionView: UICollectionView,
        layout collectionViewLayout: UICollectionViewLayout,
        sizeForItemAt indexPath: IndexPath) -> CGSize {
        let text = candidateData[indexPath.item]
        let width = text.size(withAttributes: [.font: UIFont.systemFont(ofSize: 16)]).width + 16
        return CGSize(width: max(width, 40), height: 36)
    }
}

// MARK: - CandidateCell
class CandidateCell: UICollectionViewCell {
    private let label: UILabel = {
        let label = UILabel()
        label.font = UIFont.systemFont(ofSize: 16)
        label.textAlignment = .center
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()

    override init(frame: CGRect) {
        super.init(frame: frame)
        setupView()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupView()
    }

    private func setupView() {
        contentView.addSubview(label)
        NSLayoutConstraint.activate([
            label.centerXAnchor.constraint(equalTo: contentView.centerXAnchor),
            label.centerYAnchor.constraint(equalTo: contentView.centerYAnchor)
        ])

        layer.cornerRadius = 4
        layer.borderWidth = 1
        layer.borderColor = UIColor.separator.cgColor
    }

    func configure(text: String, isSelected: Bool) {
        label.text = text
        backgroundColor = isSelected ? .systemBlue : .secondarySystemBackground
        label.textColor = isSelected ? .white : .label
    }
}

// MARK: - ImeBridge
class ImeBridge {
    static let shared = ImeBridge()

    struct CandidateResult {
        let text: String
        let code: String
        let weight: Double
    }

    private var engine: OpaquePointer?

    init() {
        engine = ime_engine_create()
    }

    func processInput(_ text: String,
        completion: @escaping ([CandidateResult]) -> Void) {
        DispatchQueue.global(qos: .userInteractive).async { [weak self] in
            guard let self = self, let engine = self.engine else {
                completion([])
                return
            }

            var count: Int32 = 0
            let candidatesPtr = ime_engine_get_candidates(engine, text, &count)

            var results: [CandidateResult] = []
            for i in 0..<Int(count) {
                let candidate = candidatesPtr[i]
                results.append(CandidateResult(
                    text: String(cString: candidate.text),
                    code: String(cString: candidate.code),
                    weight: candidate.weight
                ))
            }

            ime_candidates_free(candidatesPtr, count)
            completion(results)
        }
    }

    deinit {
        if let engine = engine {
            ime_engine_destroy(engine)
        }
    }
}

// C接口声明
@_silgen_name("ime_engine_create")
func ime_engine_create() -> OpaquePointer

@_silgen_name("ime_engine_destroy")
func ime_engine_destroy(_ engine: OpaquePointer)

@_silgen_name("ime_engine_get_candidates")
func ime_engine_get_candidates(_ engine: OpaquePointer, _ input: String,
    _ count: UnsafeMutablePointer<Int32>) -> UnsafePointer<ImeCandidate>

@_silgen_name("ime_candidates_free")
func ime_candidates_free(_ candidates: UnsafePointer<ImeCandidate>, _ count: Int32)

struct ImeCandidate {
    var text: UnsafePointer<CChar>
    var code: UnsafePointer<CChar>
    var weight: Double
}