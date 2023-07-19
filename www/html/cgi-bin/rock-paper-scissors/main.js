const inputs = document.querySelectorAll('input');
const button = document.querySelector('button');

const winsElem = document.querySelector('#wins');
const lossesElem = document.querySelector('#losses');
const tiesElem = document.querySelector('#ties');

const userChoiceElem = document.querySelector('#user-choice');
const computerChoiceElem = document.querySelector('#computer-choice');

const resultElem = document.querySelector('#status');

var myScore = 0;
var computerScore = 0;
var ties = 0;

const choiceImages = [
	{
		name: 'rock',
		src: 'rock.png'
	},
	{
		name: 'paper',
		src: 'paper.png'
	},
	{
		name: 'scissors',
		src: 'scisser.png'
	}
];

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function showLoadingAnimation() {
	const _ = new Promise(resolve => setTimeout(resolve, 2000));
}

button.addEventListener('click', async function(event) {
	event.preventDefault();

	var value;

	for (var i = 0; i < inputs.length; i++) {
		if (inputs[i].checked) {
			value = inputs[i].value;
		}
	}

	console.log(value);

	const response = await fetch(`/cgi-bin/rock-paper-scissors/backend.py?value=${value}`);
	const json = await response.json();

	var userChoice = choiceImages.find(function(choice) {
		return choice.name === value;
	});
	userChoiceElem.src = userChoice.src;
	userChoiceElem.alt = userChoice.name;
	userChoiceElem.attributes.title.value = userChoice.name;
	computerChoiceElem.src = "unknown.png";

	// Show loading animtation for 2s
	new Promise(resolve => setTimeout(() => {

		const result = json.result;

		if (result === 'win') {
			myScore++;
			resultElem.src = "winner.jpg";
		} else if (result === 'lose') {
			computerScore++;
			resultElem.src = "loser.jpg";
		} else {
			ties++;
			resultElem.src = "normal.jpg";
		}

		winsElem.textContent = myScore;
		lossesElem.textContent = computerScore;
		tiesElem.textContent = ties;

		var computerChoice = choiceImages.find(function(choice) {
			return choice.name === json.computerChoice;
		});

		computerChoiceElem.src = computerChoice.src;
		computerChoiceElem.alt = computerChoice.name;
		computerChoiceElem.attributes.title.value = computerChoice.name;
		return resolve;
	}, 1500));

	// Show the result
	computerChoiceElem.src = "loading.gif"

	


});

const labels = document.querySelectorAll('label');

for (var i = 0; i < labels.length; i++) {
	labels[i].addEventListener('click', function(event) {

		for (var j = 0; j < labels.length; j++) {
			labels[j].classList.remove('selected-choice');
		}

		this.classList.add('selected-choice');
	});
}