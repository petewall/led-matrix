const columns = 32
const rows = 8

function buildMatrix() {
    $('#matrix tbody').empty()
    for (let row = 0; row < rows; ++row) {
        const tableRow = $('<tr>')
        for (let column = 0; column < columns; ++column) {
            const cell = $('<td>').append(
                $('<div>', {class: 'ui big fitted checkbox'}).append(
                    $('<input>', {
                        id: `led-${row}-${column}`,
                        type: 'checkbox'
                    }),
                    $('<label>')
                )
            )
            tableRow.append(cell)
        }
        $('#matrix tbody').append(tableRow)
    }
}

$(document).ready(() => {
    buildMatrix()
})
  